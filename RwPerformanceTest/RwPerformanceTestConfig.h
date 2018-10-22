#pragma once
#include <string>

struct RwPerformanceTestConfig 
{
public:
	RwPerformanceTestConfig(std::string fileName);
	~RwPerformanceTestConfig();

public:
	std::string m_fileName;
	int m_time;
	std::string m_readFileName;
	std::string m_writeFilePath;

public:
	bool Params();
	void Print();
};

extern RwPerformanceTestConfig *g_pConfig;
