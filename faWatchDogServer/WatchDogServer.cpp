#include "gen-cpp/faWatchDogServer.h"
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include "RunLogFile/RunLogFile.h"
#include "faWatchDogServerConfig.h"
#include "faWatchDog.h"
#include <pthread.h>

using namespace::apache::thrift;
using namespace::apache::thrift::protocol;
using namespace::apache::thrift::transport;
using namespace::apache::thrift::server;

using boost::shared_ptr;

using namespace::faWatchDogServer;

class faWatchDogServerHandler : virtual public faWatchDogServerIf {
public:
	faWatchDogServerHandler() {
	  // Your initialization goes here
	}

	void ServiceHeart(TagServiceHeartResponse& _return, const TagServiceHeartRequest& request) {
	  // Your implementation goes here
		WriteRunLog(LOG_INFO, " get a heartbeat request, service id = %d", request.serviceID);
		
		pthread_mutex_lock(&mutex);
		if (!watchDog->updateServiceCount(request.serviceID))
		{
			WriteRunLog(LOG_INFO, " no service started, service id = %d", request.serviceID);
		}
		pthread_mutex_unlock(&mutex);
	}
};

void TSimpleServerModel(int mPort = 10086)
{
	int port = mPort;
	
	while (1)
	{
		try
		{
			shared_ptr<faWatchDogServerHandler> handler(new faWatchDogServerHandler());
			shared_ptr<TProcessor> processor(new faWatchDogServerProcessor(handler));
			shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
			shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
			shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
			
			TSimpleServer server(processor, serverTransport, transportFactory, protocolFactory);
			
			WriteRunLog(LOG_INFO, "*********************thrift server started...*********************");
			server.serve();
			
		}
		catch (TException& tx)
		{
			WriteRunLog(LOG_ERROR, " ERROR: %s", tx.what());		
		}
		catch (...)
		{
			WriteRunLog(LOG_ERROR, " Unknow Error");
		}	
		sleep(5);
	}
}

void* doServer(void* arg)
{
	int port = *(int*)arg;
	
	pthread_detach(pthread_self());
	TSimpleServerModel(port);
	pthread_exit(nullptr);
}

int main(int argc, char **argv) {
	if (argc == 2)
	{
		g_pConfig = new WatchDogServerConfig(argv[1]);
	}
	else
	{
		g_pConfig = new WatchDogServerConfig();
	}
	
	if (!g_pConfig->Paras())
	{
		WriteRunLog(LOG_INFO, " paras config file error.");
		return -1;
	}
	g_pConfig->Print();
	
	if (pthread_mutex_init(&mutex, NULL) != 0)
	{
		WriteRunLog(LOG_ERROR, " init mutex error.");
		return -1;
	}
	
	watchDog = new WatchDog();
	
	pthread_t serverThread;
	pthread_create(&serverThread, NULL, doServer, (void*)&g_pConfig->m_port);
	
	for (int i = 0; i < g_pConfig->m_service.size(); i++)
	{
		int serviceID;
		if (SearchService(g_pConfig->m_service[i].m_serviceName) != 0)
		{
			WriteRunLog(LOG_INFO,
				" exit service, service name = %s, and will restart service", 
				g_pConfig->m_service[i].m_serviceName.c_str());
			KillService(SearchService(g_pConfig->m_service[i].m_serviceName));
		}
		
		serviceID = StartService(g_pConfig->m_service[i].m_serviceName, 
			g_pConfig->m_service[i].m_startCommand);

		Service service = {
			g_pConfig->m_service[i].m_serviceName, 
			g_pConfig->m_service[i].m_startCommand,
			0 
		};
		
		if (!(watchDog->addService(serviceID, service)))
		{
			continue;	
		}
	}
	watchDog->print();
	
	HeartBeatCheck((void*)&watchDog->m_services);
	
	pthread_mutex_destroy(&mutex);
	
	return 0;
}
