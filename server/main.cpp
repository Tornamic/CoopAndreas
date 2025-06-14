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

	int port = 0;	
	CConfigFile serverconfig;
	if(serverconfig.InitConfigFile() == false)
	{
		exit(EXIT_FAILURE);
	}
	if(serverconfig.GetConfigFileVariable_Port(port) == false)
	{
		exit(EXIT_FAILURE);
	}
	
	
	CNetwork::Init(port);
	return 0;
}