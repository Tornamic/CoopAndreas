#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>

#if defined (_WIN32)
#include <winsock2.h>
#include <windows.h>
#endif

#include "include/enet/enet.h"
#include "include/dini/iem-dracoblue-implementation.h"
#include "core/CNetwork.h"

#include "core/CConfigFile.h"
#include "../shared/semver.h"

int main(int argc, char *argv[])
{
#if defined (_WIN32)
	SetConsoleTitleA("CoopAndreas Server");
#endif
	printf("[!] : Support:\n");
	printf("- https://github.com/Tornamic/CoopAndreas\n");
	printf("- https://discord.gg/TwQsR4qxVx\n");
	printf("- coopandreasmod@gmail.com\n\n");

	printf("[!] : CoopAndreas Server \n");
#ifdef DEBUG
	char config[] = "Debug";
#else
	char config[] = "Release";
#endif

	printf("[!] : Version : %s, %s %s %s %s\n", COOPANDREAS_VERSION, config, sizeof(void*) == 8 ? "x64" : "x86", __DATE__, __TIME__);
#if defined (_WIN32)
	printf("[!] : Platform : Microsoft Windows \n");
#else
	printf("[!] : Platform : GNU/Linux | BSD \n");
#endif

	// this code is just an expirement of server config files
	int port = 0;
	char ipaddress[DINI_MODULE_MAX_STRING_SIZE] = {0};	// 255 size or 256 i guess , i forget
	CConfigFile serverconfig;
	if(serverconfig.InitConfigFile() == false)
	{
		printf("1");
		exit(EXIT_FAILURE);
	}
	if(serverconfig.GetConfigFileVariable_IPAddress(ipaddress) == false)
	{
		printf("2");
		exit(EXIT_FAILURE);
	}
	if(serverconfig.GetConfigFileVariable_Port(port) == false)
	{
		printf("3");
		exit(EXIT_FAILURE);
	}
	if(serverconfig.GetConfigFileVariable_Players(max_server_slots) == false)
	{
		printf("4");
		exit(EXIT_FAILURE);
	}
	
	printf("\n[!] : Max number of players in server is %d\n", max_server_slots);
	
	unsigned short us_port = port;
	CNetwork::Init(ipaddress, us_port, max_server_slots);
	return 0;
}