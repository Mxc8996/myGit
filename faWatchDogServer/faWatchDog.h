#pragma once
#include <map>
#include <string>
#include <time.h>

typedef struct Service
{
	std::string serviceName;
	std::string startCommand;
	int count;
}Service;

std::string GetTime();
int StrToInt(const char* sServiceID);
int StartService(std::string serviceName, std::string startCommand);
int SearchService(std::string serviceName);
bool KillService(int serviceID);
void* HeartBeatCheck(void* arg);

class WatchDog
{
public:
	WatchDog();
	~WatchDog();

public:
	bool addService(int serviceID, Service service);
	bool updateServiceCount(int serviceID);
	void print();
	
public:
	std::map<int, Service> m_services;
};

extern pthread_mutex_t mutex;


