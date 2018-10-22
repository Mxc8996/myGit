#pragma once
#include "faWatchDog.h"
#include <string>
#include <vector>

typedef struct tagServiceConf
{
	std::string m_serviceName;
	std::string m_startCommand;
}tagServiceConf;

class WatchDogServerConfig
{
public:
	int m_port;
	std::string m_strFileName;
	std::vector<tagServiceConf> m_service;
	unsigned int m_timeOut;
	
public:
	WatchDogServerConfig(std::string strFileName = "WatchDogServer.json");
	~WatchDogServerConfig();
	
private:
	WatchDogServerConfig(const WatchDogServerConfig &config);
	const WatchDogServerConfig& operator =(const WatchDogServerConfig &config);

public:
	bool Paras();
	void Print();
};

extern WatchDogServerConfig *g_pConfig;
extern WatchDog *watchDog;