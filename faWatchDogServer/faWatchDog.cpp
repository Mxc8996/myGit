#include "faWatchDog.h"
#include "RunLogFile/RunLogFile.h"
#include <fstream>
#include <unistd.h>
#include <limits>
#include "faWatchDogServerConfig.h"

#define COUNT_TIMES 4

pthread_mutex_t mutex;

WatchDog::WatchDog()
{
}

WatchDog::~WatchDog()
{
}

std::string GetTime()
{
	time_t timeP;
	time(&timeP);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timeP));
	
	return tmp;
}

int StrToInt(const char* sServiceID)
{
	int serviceID = 0;

	if(sServiceID != NULL)
	{
		const char* str = sServiceID;
		while (*str != '\0')
		{
			if (*str >= '0' && *str <= '9')
			{
				serviceID = serviceID * 10 + (*str - '0');
				if (serviceID > std::numeric_limits<int>::max())
				{
					serviceID = 0;
					break;
				}
				++str;
			}
			else if (*str == '\n')
			{
				break;
			}
			else
			{
				serviceID = 0;
				break;
			}
		}
	}

	return serviceID;
}

int StartService(std::string serviceName, std::string startCommand)
{
	int serviceID;
	const char *startCmd;
	FILE *fp;
	startCmd = startCommand.c_str();
	
	if ((fp = popen(startCmd, "r")) == NULL)
	{
		WriteRunLog(LOG_ERROR,
			"start service error, service name = %s, start command = %s", 
			serviceName.c_str(),
			startCommand.c_str());
		return 0;
	}
	
	pclose(fp);
	
	serviceID = SearchService(serviceName);
	
	if (serviceID == 0)
	{
		WriteRunLog(LOG_INFO, "service start faild, service name = %s", serviceName.c_str());
	}
	
	return serviceID;
}

int SearchService(std::string serviceName)
{
	int serviceID;
	char searchCmd[255];
	char buf[100] = { '\0' };
	FILE *fp;
	
	sprintf(searchCmd, "ps -ef | grep %s | grep -v grep | awk '{print $2}'", serviceName.c_str());
	
	if ((fp = popen(searchCmd, "r")) == NULL)
	{
		WriteRunLog(LOG_ERROR,
			"search service error, service name = %s",
			serviceName.c_str());
		return 0;
	}
	
	while (fgets(buf, sizeof(buf) -1, fp) != NULL)
	{
		printf("search result: service name = %s, search info = %s", serviceName.c_str(), buf);
	}

	pclose(fp);
	serviceID = StrToInt(buf);

	return serviceID;
}

bool KillService(int serviceID)
{
	char killCmd[100] = { '\0' };
	FILE *fp;
	
	sprintf(killCmd, "kill -9 %d", serviceID);
	
	if ((fp = popen(killCmd, "r")) == NULL)
	{
		WriteRunLog(LOG_ERROR, "kill service error.");	
		return false;
	}
	
	pclose(fp);
	
	return true;
}

bool WatchDog::updateServiceCount(int serviceID)
{
	std::map<int, Service>::iterator service = m_services.begin();
	for (; service != m_services.end();)
	{
		if (service->first == serviceID)
		{
			service->second.count = 0;
			WriteRunLog(LOG_INFO, "service count has been reset, service name = %s", 
				service->second.serviceName.c_str());
			return true;
		}
		else
		{
			++service;
		}
	}
	if (service == m_services.end())
	{
		WriteRunLog(LOG_INFO, "service count reset faild, service id = %d not exit", serviceID);
		return false;
	}
	return true;
}

bool WatchDog::addService(int serviceID, Service service)
{
	if (serviceID == 0)
	{
		WriteRunLog(LOG_INFO, "service not started, servicename = %s", service.serviceName.c_str());
		return false;
	}
	m_services.insert(std::pair<int, Service>(serviceID, service));
	return true;
}

void* HeartBeatCheck(void *arg)
{
	WriteRunLog(LOG_INFO, "**********************HertBeat check thread start*********************.");
	
	std::map<int, Service> *services = (std::map<int, Service>*)arg;
	
	while (1)
	{
		std::map<int, Service>::iterator service = services->begin();
		
		for ( ; service != services->end(); )
		{
			printf("%d\n", service->second.count);
			
			if (service->second.count == COUNT_TIMES)
			{
				
				Service *tag = &service->second;
				
				int newID = 0;
				
				int serviceID = SearchService(tag->serviceName);
				if (serviceID != 0)
				{
					KillService(serviceID);
					newID = StartService(service->second.serviceName, service->second.startCommand);
				}
				else
				{
					newID = StartService(service->second.serviceName, service->second.startCommand);
				}
				
				if (newID != 0)
				{
					tag->count = 0;				
					services->insert(std::pair<int, Service>(newID, *tag));
					service = services->erase(service);
					
					WriteRunLog(LOG_INFO, " %s, service has been restart, service name = %s, service id = %d",
						GetTime().c_str(),
						tag->serviceName.c_str(),
						newID);
				}
				else
				{
					WriteRunLog(LOG_INFO, " %s, service restart faild, service name = %s", 
						GetTime().c_str(),
						service->second.serviceName.c_str());
					break;
				}

			}
			else if (service->second.count >= 0 && service->second.count < COUNT_TIMES)
			{
				pthread_mutex_lock(&mutex);
				service->second.count += 1;
				pthread_mutex_unlock(&mutex);
				++service;
			}
			else
			{
				++service;
			}
		}
		sleep((g_pConfig->m_timeOut/COUNT_TIMES)+1);
	}
}

void WatchDog::print()
{
	WriteRunLog(LOG_INFO, " *********************all service info********************");
	std::map<int, Service>::iterator service = m_services.begin();
	for (; service != m_services.end();)
	{
		WriteRunLog(LOG_INFO, 
			"serviceID = %d, "
			"serviceName = %s, "
			"startCommand = %s, "
			"count = %d"
			, service->first
			, service->second.serviceName.c_str()
			, service->second.startCommand.c_str()
			, service->second.count);
		++service;
	}
}
