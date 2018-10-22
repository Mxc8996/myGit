#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <limits>
#include <iostream>
#include "RunLogFile/RunLogFile.h"
#include "faProtectProcessConfig.h"

using namespace std;

volatile sig_atomic_t _running = 1;

void sigterm_handler(int arg)
{
	_running = 0;
}

int StrToInt(const char* sProcessID)
{
	int processID = 0;

	if (sProcessID != NULL)
	{
		const char* str = sProcessID;
		while (*str != '\0')
		{
			if (*str >= '0' && *str <= '9')
			{
				processID = processID * 10 + (*str - '0');
				if (processID > std::numeric_limits<int>::max())
				{
					processID = 0;
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
				processID = 0;
				break;
			}
		}
	}

	return processID;
}

int SearchServer(std::string serverName)
{
	int serviceID;
	char searchCmd[255];
	char buf[100] = { '\0' };
	FILE *fp;
	
	sprintf(searchCmd, "ps -ef | grep %s | grep -v grep | awk '{print $2}'", serverName.c_str());
	
	if ((fp = popen(searchCmd, "r")) == NULL)
	{
		WriteRunLog(LOG_ERROR,
			"search server process error, server name = %s",
			serverName.c_str());
		return 0;
	}
	
	while (fgets(buf, sizeof(buf) - 1, fp) != NULL)
	{
		printf("search result: server name = %s, search info = %s", serverName.c_str(), buf);
	}

	pclose(fp);
	serviceID = StrToInt(buf);

	return serviceID;
}

int StartServer(std::string serverName, std::string startCommand)
{
	int serverID;
	const char *startCmd;
	FILE *fp;
	startCmd = startCommand.c_str();
	
	if ((fp = popen(startCmd, "r")) == NULL)
	{
		WriteRunLog(LOG_ERROR,
			"start server process error, server name = %s, start command = %s", 
			serverName.c_str(),
			startCommand.c_str());
		return 0;
	}
	
	pclose(fp);
	
	serverID = SearchServer(serverName);
	
	if (serverID == 0)
	{
		WriteRunLog(LOG_INFO, "service start faild, service name = %s", serverName.c_str());
	}
	
	return serverID;
}

bool KillServer(int processID)
{
	char killCmd[100] = { '\0' };
	FILE *fp;
	
	sprintf(killCmd, "kill -9 %d", processID);
	
	if ((fp = popen(killCmd, "r")) == NULL)
	{
		WriteRunLog(LOG_ERROR, "kill service error.");	
		return false;
	}
	
	pclose(fp);
	
	return true;
}

int main(int argc, char *argv[])
{	
	pid_t pid;
	
	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	
	pid = fork();

	if (pid < 0)
	{
		WriteRunLog(LOG_ERROR, "fork error!");
		return -1;
	}
	else if (pid > 0)
	{
		exit(0);
	}
	
	setsid();
	
	char curPath[1024];
	if (getcwd(curPath, sizeof(curPath)) == NULL)
	{
		WriteRunLog(LOG_ERROR, "getcwd error!");
		return -1;
	}
	else
	{
		chdir(curPath);
		WriteRunLog(LOG_INFO, "set current psth success, path = %s", curPath);
	}
	
	umask(0);
	/*
	for (int i = 3; i < 65535; i++)
	{
		close(i);
	}*/
	
	signal(SIGTERM, sigterm_handler);
	
	WriteRunLog(LOG_INFO, " protect process to work, and use kill -9 PID to terminate.");
	
	g_pConfig = new faProtectProcessConfig();
	if (!g_pConfig->Paras())
	{
		WriteRunLog(LOG_INFO, " paras falid.");
		return -1;
	}
	g_pConfig->Print();
	
	WriteRunLog(LOG_INFO, " watdog server name = %s, start command = %s", 
		g_pConfig->m_strServerName.c_str(), g_pConfig->m_strStartCommand.c_str());
	
	
	if (SearchServer(g_pConfig->m_strServerName) != 0)
	{
		KillServer(SearchServer(g_pConfig->m_strServerName));
	}
	
	if (StartServer(g_pConfig->m_strServerName, g_pConfig->m_strStartCommand) == 0)
	{
		WriteRunLog(LOG_INFO, " server [%s] process start faild.", g_pConfig->m_strServerName.c_str());
		return -1;
	}
	
	while (_running)
	{
		if (SearchServer(g_pConfig->m_strServerName) == 0)
		{
			WriteRunLog(LOG_INFO, "server [%s] process has stoped, restarting server process..", g_pConfig->m_strServerName.c_str());
			if (StartServer(g_pConfig->m_strServerName, g_pConfig->m_strStartCommand) == 0)
			{
				WriteRunLog(LOG_INFO, "server [%s] process restart faild.", g_pConfig->m_strServerName.c_str());
				break;
			}
		}
		usleep(5 * 1000 * 1000);
	}
	
	if (!_running)
	{
		if (SearchServer(g_pConfig->m_strServerName) != 0)
		{
			KillServer(SearchServer(g_pConfig->m_strServerName));
		}
		WriteRunLog(LOG_INFO, "protect process terminate.");
	}
	
	return 0;
}