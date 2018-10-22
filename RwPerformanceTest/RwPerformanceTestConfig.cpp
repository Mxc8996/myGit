#include "RwPerformanceTestConfig.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "RunLogFile/RunLogFile.h"


RwPerformanceTestConfig* g_pConfig = nullptr;

RwPerformanceTestConfig::RwPerformanceTestConfig(std::string fileName) 
	: m_fileName(fileName)
{

}

RwPerformanceTestConfig::~RwPerformanceTestConfig()
{

}

bool RwPerformanceTestConfig::Params()
{
	std::fstream ifs(m_fileName);
	if (!ifs.is_open())
	{
		WriteRunLog(LOG_INFO, "open file error, file = %s", m_fileName.c_str());
		return false;
	}
	std::ostringstream ostr;
	ostr << ifs.rdbuf();
	std::string strJson = ostr.str();
	
	rapidjson::Document doc;
	doc.Parse<0>(strJson.c_str());
	assert(!doc.HasParseError());
	
	m_time = doc["Time"].GetInt();
	m_readFileName = doc["ReadFileName"].GetString();
	m_writeFilePath = doc["WriteFilePath"].GetString();

	return true;
}

void RwPerformanceTestConfig::Print()
{
	WriteRunLog(LOG_INFO,
		"*********************config info********************\n"
		"Time = %d\n"
		"ReadFileName = %s\n"
		"WriteFilePath = %s\n",
		m_time,
		m_readFileName.c_str(),
		m_writeFilePath.c_str());
}