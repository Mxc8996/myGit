#include "faProtectProcessConfig.h"
#include "RunLogFile/RunLogFile.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <fstream>
#include <sstream>

faProtectProcessConfig* g_pConfig = nullptr;

faProtectProcessConfig::faProtectProcessConfig(std::string strFileName)
	: m_strFileName(strFileName)
{
	
}

faProtectProcessConfig::~faProtectProcessConfig()
{
	
}

bool faProtectProcessConfig::Paras()
{
	std::ifstream ifs(m_strFileName.c_str());
	
	if (!ifs.is_open())
	{
		WriteRunLog(LOG_INFO," open file error, fileName = %s", m_strFileName.c_str());
		return false;
	}
	std::ostringstream ostr;
	ostr << ifs.rdbuf();
	std::string strJson = ostr.str();
	
	rapidjson::Document doc;
	doc.Parse<0>(strJson.c_str());
	assert(!doc.HasParseError());
	
	m_strServerName = doc["ServerName"].GetString();
	m_strStartCommand = doc["StartCommand"].GetString();

	return true;
}

void faProtectProcessConfig::Print()
{
	WriteRunLog(LOG_INFO,
		"*****************config info*****************\n"
		"m_strServerName = %s"
		"m_strStartCommand = %s"
		, m_strServerName.c_str()
		, m_strStartCommand.c_str());
}