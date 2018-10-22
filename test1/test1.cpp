#include <stdio.h>
#include <unistd.h>
#include "WatchDogClient.h"

int main(int argc, char* argv[])
{
	std::string host = "192.168.1.72";
	int port = 10086;
	
	HeartBeatCheck(host, port);
	
	for (int i =0; i < 10000; i++)
	{
		printf("I am test1, service id = %d\n", getpid());
		sleep(1);
	}
	sleep(10);
	
	return 0;
}