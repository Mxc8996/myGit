#include <time.h>
#include <stdio.h>
#include <io.h>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "RwPerformanceTestConfig.h"
#include "RunLogFile/RunLogFile.h"

struct fileData {
	long BufLen;
	char* Buf;
};

void casMkdir(const char* dir)
{
	int nIndex = 1;
	char* tmp = (char*)dir;
	while (nIndex < strlen(dir) + 1)
	{
		if (*(dir + nIndex) == '/' || *(dir + nIndex) == '\0')
		{
			char parent[256] = { 0 };
			strncpy(parent, dir, nIndex);
			if (0 != access(parent, 6))
			{
				mkdir(parent, 0755);
			}
		}
		nIndex++;
	}
}

void  WriteFile(void* pParam)
{
	fileData *pTmp;
	pTmp = (fileData*)pParam;
	time_t tBegin = time(0), tEnd = time(0);
	int nIndex = 0;
	while (1)
	{
		time_t t = time(nullptr);
		struct tm* pTime = gmtime(&t);
		char szFilePath[256] = { 0 };
		sprintf(szFilePath,
			"%s/%02d%02d/%02d:%02d",
			g_pConfig->m_writeFilePath.c_str(),
			pTime->tm_mon + 1,
			pTime->tm_mday,
			pTime->tm_hour+8,
			pTime->tm_min);
	
		if (access(szFilePath, 0))
		{
			casMkdir(szFilePath);
		}
		char szFileName[256] = { 0 };
		sprintf(szFileName, "%s/%d.jpg", szFilePath, nIndex);
		FILE *pFile;
		pFile = fopen(szFileName, "w+b");
		if (pFile)
		{
			fwrite(pTmp->Buf, pTmp->BufLen, 1, pFile);
			fclose(pFile), pFile = nullptr;
		}
		else
		{
			WriteRunLog(LOG_INFO, "open file error. filename =", szFileName);
		}
		nIndex++;

		tEnd = time(0);
		if ((tEnd - tBegin) == g_pConfig->m_time)
		{
			tEnd = time(0);
			WriteRunLog(LOG_INFO, " spend time = %ds, nIndex = %d", g_pConfig->m_time, nIndex);
			tBegin = tEnd;
			nIndex = 0;
		}
		/*
		if (0 == nIndex % 100)
		{
			tEnd = time(0);
			LOG(INFO) << __TIME__ << ",nIndex = " << nIndex << ", time = " << tEnd - tBegin << "s" << std::endl;
			tBegin = tEnd;
		}*/
	}
}

int main(int argc, char* argv[])
{
	if (argc == 2)
	{
		g_pConfig = new RwPerformanceTestConfig(argv[1]);
	}
	else
	{
		std::string fileName = "./config.json";
		g_pConfig = new RwPerformanceTestConfig(fileName);
	}

	if (!g_pConfig->Params())
	{
		WriteRunLog(LOG_INFO, " init faild.");
		return -1;
	}

	g_pConfig->Print();


	fileData *pData = new fileData;
	FILE *fp;
	fp = fopen(g_pConfig->m_readFileName.c_str(), "rb");
	if (!fp)
	{
		WriteRunLog(LOG_INFO, " open file faild, file path = ", g_pConfig->m_readFileName.c_str());
		return -1;
	}
	fseek(fp, 0, SEEK_END);
	pData->BufLen = ftell(fp);
	rewind(fp);

	pData->Buf = (char*)malloc(pData->BufLen * sizeof(char));
	fread(pData->Buf, pData->BufLen, 1, fp);
	fclose(fp);
	
	WriteFile((void*)pData);

	char ch; 
	while (1)
	{
		std::cin >> ch;
	}

	if (pData) 
	{
		delete pData, pData = nullptr;
	}
	return 0;
}
