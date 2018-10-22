#include "faWatchDogServerConfig.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "RunLogFile/RunLogFile.h"
#include "faWatchDog.h"
#include <fstream>
#include <sstream>

#include <iostream>

WatchDogServerConfig *g_pConfig = nullptr;
WatchDog *watchDog = nullptr;

WatchDogServerConfig::WatchDogServerConfig(std::string strFileName)
	: m_strFileName(strFileName)
{
}

WatchDogServerConfig::~WatchDogServerConfig()
{
}

WatchDogServerConfig::WatchDogServerConfig(const WatchDogServerConfig &config)
{
}

const WatchDogServerConfig &WatchDogServerConfig::operator=(const WatchDogServerConfig &config)
{
}

bool WatchDogServerConfig::Paras()
{
	bool bRet = true;
	std::ifstream ifs(m_strFileName.c_str());
	tagServiceConf tag; 
	
	if (!ifs.is_open())
	{
		WriteRunLog(LOG_INFO, " open file error, file = %s", m_strFileName.c_str());
		bRet = false;
		return bRet;
	}
	std::ostringstream ostr;
	ostr << ifs.rdbuf();
	std::string strJson = ostr.str();
	
	rapidjson::Document doc;
	doc.Parse<0>(strJson.c_str());
	assert(!doc.HasParseError());

	m_port = doc["Port"].GetInt();
	m_timeOut = doc["TimeOut"].GetInt();

	rapidjson::Value & serviceArr = doc["ServiceInfo"];

	if (!serviceArr.IsArray())
	{
		WriteRunLog(LOG_INFO, " 'Serviceinfo' is not a array.");
		bRet = false;
		return bRet;
	}
	
	for (int i = 0; i < serviceArr.Size(); i++)
	{
		tag.m_serviceName = serviceArr[i]["ServiceName"].GetString();
		tag.m_startCommand = serviceArr[i]["StartCommand"].GetString();
		m_service.push_back(tag);
	}
	
	return bRet;
}

void WatchDogServerConfig::Print()
{
	WriteRunLog(LOG_INFO, " ********************config info*******************");
	WriteRunLog(LOG_INFO, " m_port = %d\n m_timeOut = %d", m_port, m_timeOut);
	for (int i = 0; i < m_service.size(); i++)
	{
		WriteRunLog(LOG_INFO, " %d: serviceName = %s, startCommand = %s", 
			i, 
			m_service[i].m_serviceName.c_str(),
			m_service[i].m_startCommand.c_str());
	}
}
