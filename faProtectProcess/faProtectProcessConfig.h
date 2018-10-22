#pragma once
#include <string>

class faProtectProcessConfig
{
public:
	std::string m_strFileName;
	std::string m_strServerName;
	std::string m_strStartCommand;
	
public:
	faProtectProcessConfig(std::string fileName = "faProtectProcessConfig.json");
	~faProtectProcessConfig();
	
public:
	bool Paras();
	void Print();
};

extern faProtectProcessConfig* g_pConfig;