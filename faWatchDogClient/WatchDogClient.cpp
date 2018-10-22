#include <iostream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TMultiplexedProtocol.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PosixThreadFactory.h>

#include "gen-cpp/faWatchDogServer.h"
#include "WatchDogClient.h"
//#include "RunLogFile/RunLogFile.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using boost::shared_ptr;
using namespace::faWatchDogServer;

typedef struct tagConf
{
	std::string m_host;
	int m_port;
}tagConf;

tagConf m_tag;

void TSimpleServerModel(void* arg)
{
	m_tag = *(tagConf*)arg;
	std::string host = m_tag.m_host;
	int port = m_tag.m_port;
	
	std::cout << host << "\n" << port << std::endl;
	
	//WriteRunLog(LOG_INFO, "********************watchDogClient*******************");
	printf("********************watchDogClient*******************");
	
	boost::shared_ptr<TSocket> socket(new TSocket(host, port));
	boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	
	boost::shared_ptr<TMultiplexedProtocol> cMp1(new TMultiplexedProtocol(protocol, "WatchDogServer"));
	
	faWatchDogServer::faWatchDogServerClient client(protocol);
	
	try
	{
		while (1)
		{
			transport->open();
			TagServiceHeartRequest hertRequest;
			TagServiceHeartResponse hertResponse;
			
			hertRequest.serviceID = getpid();
			printf("This service id = %d", hertRequest.serviceID);
			//WriteRunLog(LOG_INFO, " This service id = %d", hertRequest.serviceID);
			
			client.ServiceHeart(hertResponse, hertRequest);

			transport->close();
			sleep(5);
		}
	}
	catch (TException &tx)
	{
		//WriteRunLog(LOG_ERROR, " ERROR: %s", tx.what());
		printf("ERROR IS: %s\n", tx.what());
	}
	catch (...)
	{
		//WriteRunLog(LOG_ERROR, " Unknow error.");
		printf("Unknow error\n");
	}
}

void* doWork(void* arg)
{
	pthread_detach(pthread_self());
	TSimpleServerModel(arg);
	pthread_exit(nullptr);
}

void HeartBeatCheck(std::string host, int port)
{
	m_tag.m_host = host;
	m_tag.m_port = port;
	
	pthread_t hertBeat;
	pthread_create(&hertBeat, NULL, doWork, (void*)&m_tag);

}
