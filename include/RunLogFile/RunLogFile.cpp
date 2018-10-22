#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <iomanip>
#include <time.h>
#include <chrono>
#include <pthread.h>
#include <thread>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/layout.h>
#include "RunLogFile.h"
using namespace log4cplus;

class CMutex
{
public:
	CMutex() { pthread_mutex_init(&m_mutex, NULL); }
	~CMutex() { pthread_mutex_destroy(&m_mutex); }

	virtual void Lock() const { pthread_mutex_lock(&m_mutex); }
	virtual void UnLock() const { pthread_mutex_unlock(&m_mutex); }

private:
	mutable pthread_mutex_t m_mutex;
};

class CMyLogLock
{
public:
	CMyLogLock(const CMutex&lock) :m_lock(lock)
	{
		m_lock.Lock();
	}
	~CMyLogLock()
	{
		m_lock.UnLock();
	}

private:
	const CMutex& m_lock;
};

class visLogFile
{
public:
	visLogFile();
	~visLogFile();

public:
	enum enLogLevel
	{
		_LOG_INFO = LOG_INFO,
		_LOG_DEBUG = LOG_DEBUG,
		_LOG_TRACE = LOG_TRACE,
		_LOG_ERROR = LOG_ERROR,
		_LOG_WARN = LOG_WARN,
		_LOG_FATAL = LOG_FATAL,
		_LOG_OFF = LOG_OFF
	};

	Logger m_Logger;
	Logger m_ErrorLogger;
	SharedAppenderPtr m_pAppend;
	SharedAppenderPtr m_pConsoleAppend;
	std::auto_ptr<Layout> m_pLayout;

	char m_szInstanceName[48];
	char m_szErrorInstanceName[48];
	char m_szModulePath[_MAX_PATH];
	char m_szModuleName[_MAX_PATH];
	char m_szLogFile[_MAX_PATH];
	std::string m_strPattern;
	int m_nMaxLogSize;
	char m_szLogPath[_MAX_PATH];
	char m_szErrorLogFile[_MAX_PATH];

public:
	void WriteLog(int level, const char* buffer);
	long GetFileSize(const char* pFile);
	void InitLog4CPlus(bool bReInit, const char* pLogName, const char* pInstanceName, log4cplus::Logger& logger);
};

visLogFile::visLogFile()
{
	memset(m_szModulePath, 0, sizeof m_szModulePath);
	memset(m_szModuleName, 0, sizeof m_szModuleName);
	memset(m_szLogFile, 0, sizeof m_szLogFile);
	memset(m_szLogPath, 0, sizeof m_szLogPath);
	m_strPattern = "%p-%D{%y/%m/%d %H:%M:%S-%Q}{ThreadID:%t}%m\r\n";
	m_nMaxLogSize = 500 * 1024 * 1024;//500M

	strcpy(m_szInstanceName, "log");
	strcpy(m_szErrorInstanceName, "errlog");

	int nCount = readlink("/proc/self/exe", m_szModulePath, _MAX_PATH);
	if (nCount < 0 || nCount > _MAX_PATH)
	{
		printf("readlink failed.");
	}
	std::string strTmpPath = m_szModulePath;
	memset(m_szModulePath, 0, sizeof(m_szModulePath));
	int nFind = strTmpPath.find_last_of("/");
	if (nFind != std::string::npos)
	{
		std::string strTmpName = strTmpPath;
		strTmpPath.erase(nFind + 1, strTmpPath.length() - nFind - 1);
		strTmpName.erase(0, nFind + 1);
		strncpy(m_szModuleName, strTmpName.c_str(), strTmpName.length());
		strncpy(m_szModulePath, strTmpPath.c_str(), strTmpPath.length());
	}
	strcat(m_szModulePath, "../");

	time_t tm = time(nullptr);
	struct tm* p = localtime(&tm);
	sprintf(m_szLogPath, "%slog/%04d%02d/%02d"
		, m_szModulePath
		, p->tm_year + 1900, p->tm_mon + 1, p->tm_mday
	);

	sprintf(m_szLogFile, "%s/%s.log"
		, m_szLogPath
		, m_szModuleName
	);

	sprintf(m_szErrorLogFile, "%s/%s.log"
		, m_szLogPath
		, "errlog"
	);

	InitLog4CPlus(false, m_szLogFile, m_szInstanceName, m_Logger);
	//InitLog4CPlus(false, m_szErrorLogFile, m_szErrorInstanceName, m_ErrorLogger);

	//char szStdErrLogFile[256] = { 0 };
	//sprintf(szStdErrLogFile, "%slog/%04d%02d/%02d/stderr.log"
	//	, m_szModulePath
	//	, p->tm_year + 1900, p->tm_mon + 1, p->tm_mday
	//	);
	//if (!freopen(szStdErrLogFile, "a", stderr))
	//{
	//	printf("freopen error.szStdErrLogFile=%s", szStdErrLogFile);
	//}
}

visLogFile::~visLogFile()
{
	//if (stderr)
	//	fclose(stderr);
}

long visLogFile::GetFileSize(const char* filename)
{
	unsigned long size = -1;
	struct stat statbuff;
	if (stat(filename, &statbuff) < 0)
		return -1;//不存在
	else
		return statbuff.st_size;
}

void visLogFile::InitLog4CPlus(bool bReInit, const char* pLogName, const char* pInstanceName, log4cplus::Logger& logger)
{
	m_pAppend = new RollingFileAppender(pLogName, m_nMaxLogSize, 3, true, true);
	m_pConsoleAppend = new ConsoleAppender();
	logger = Logger::getInstance(pInstanceName);
	m_pLayout = std::auto_ptr<Layout>(new PatternLayout(m_strPattern));
	m_pAppend->setLayout(m_pLayout);
	if (bReInit)
		logger.removeAllAppenders();
	logger.addAppender(m_pAppend);
	//此处可以配置
	logger.addAppender(m_pConsoleAppend);
	logger.setLogLevel(ALL_LOG_LEVEL);
}


void visLogFile::WriteLog(int level, const char* buffer)
{
	time_t tm = time(nullptr);
	struct tm* p = localtime(&tm);
	char szTmpLogPath[_MAX_PATH] = { 0 };
	sprintf(szTmpLogPath, "%slog/%04d%02d/%02d"
		, m_szModulePath
		, p->tm_year + 1900, p->tm_mon + 1, p->tm_mday
	);
	if (strcmp(szTmpLogPath, m_szLogPath) != 0)
	{
		strcpy(m_szLogPath, szTmpLogPath);

		sprintf(m_szLogFile, "%s/%s.log"
			, m_szLogPath
			, m_szModuleName
		);

		sprintf(m_szErrorLogFile, "%s/%s.log"
			, m_szLogPath
			, "errlog"
		);

		InitLog4CPlus(true, m_szLogFile, m_szInstanceName, m_Logger);
		//InitLog4CPlus(true, m_szErrorLogFile, m_szErrorInstanceName, m_ErrorLogger);

		//if (stderr)
		//	fclose(stderr);
		//char szStdErrLogFile[256] = { 0 };
		//sprintf(szStdErrLogFile, "%slog/%04d%02d/%02d/stderr.log"
		//	, m_szModulePath
		//	, p->tm_year + 1900, p->tm_mon + 1, p->tm_mday
		//);
		//freopen(szStdErrLogFile, "a", stderr);
	}
	switch (level)
	{
	case _LOG_INFO:
		LOG4CPLUS_INFO(m_Logger, buffer);
		break;

	case _LOG_DEBUG:
		LOG4CPLUS_DEBUG(m_Logger, buffer);
		break;

	case _LOG_TRACE:
		LOG4CPLUS_TRACE(m_Logger, buffer);
		break;

	case _LOG_ERROR:
		LOG4CPLUS_ERROR(m_Logger, buffer);
		//LOG4CPLUS_ERROR(m_ErrorLogger, buffer);
		break;

	case _LOG_WARN:
		LOG4CPLUS_WARN(m_Logger, buffer);
		//LOG4CPLUS_WARN(m_ErrorLogger, buffer);
		break;

	case _LOG_FATAL:
		LOG4CPLUS_FATAL(m_Logger, buffer);
		//LOG4CPLUS_FATAL(m_ErrorLogger, buffer);
		break;

	case _LOG_OFF:
		break;

	default:
		break;
	}

}

/* ****************************************************** interface ****************************************************** */

CMutex g_UpdateDirLock;
visLogFile * g_pLogfile = NULL;

void iWriteRunLog(const char* file, int line, int level, const char* format, ...)
{
	CMyLogLock lock(g_UpdateDirLock);
	if (!g_pLogfile)
		g_pLogfile = new visLogFile();
	char buffer[4096];
	int len = sprintf(buffer, "[file:%s,line=%d]-->", file, line);

	va_list ap;
	va_start(ap, format);
	vsnprintf(buffer + len, (sizeof buffer) - len, format, ap);
	va_end(ap);
	g_pLogfile->WriteLog(level, buffer);
}

void SetConsole(bool bConsole)
{
	CMyLogLock lock(g_UpdateDirLock);
	if (!g_pLogfile)
		g_pLogfile = new visLogFile();

	if (!bConsole)
		g_pLogfile->m_Logger.removeAppender(g_pLogfile->m_pConsoleAppend);
	else
		g_pLogfile->m_Logger.addAppender(g_pLogfile->m_pConsoleAppend);
}

void SetLogFileName(const char* logname)
{
	CMyLogLock lock(g_UpdateDirLock);
	if (!g_pLogfile)
		g_pLogfile = new visLogFile();

	time_t tm = time(nullptr);
	struct tm* p = localtime(&tm);
	char szOldLogFile[_MAX_PATH] = { 0 };
	sprintf(szOldLogFile, "%slog/%04d%02d/%02d/%s.log"
		, g_pLogfile->m_szModulePath
		, p->tm_year + 1900, p->tm_mon + 1, p->tm_mday
		, g_pLogfile->m_szModuleName
	);

	if (g_pLogfile->GetFileSize(szOldLogFile) == 0)
	{
		remove(szOldLogFile);
	}

	char szTmpLogPath[_MAX_PATH] = { 0 };
	sprintf(szTmpLogPath, "%slog/%04d%02d/%02d"
		, g_pLogfile->m_szModulePath
		, p->tm_year + 1900, p->tm_mon + 1, p->tm_mday
	);

	strcpy(g_pLogfile->m_szLogPath, szTmpLogPath);

	strcpy(g_pLogfile->m_szModuleName, logname);

	sprintf(g_pLogfile->m_szLogFile, "%s/%s.log"
		, g_pLogfile->m_szLogPath
		, g_pLogfile->m_szModuleName
	);
	//printf("m_szLogFile=%s\n", g_pLogfile->m_szLogFile);

	g_pLogfile->InitLog4CPlus(true, g_pLogfile->m_szLogFile, g_pLogfile->m_szInstanceName, g_pLogfile->m_Logger);
}
