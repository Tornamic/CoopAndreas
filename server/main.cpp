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
#include "core/CNetwork.h"

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

	char c_port[6+2];
	short port = 0;	
	printf("\n[!] : Enter the port for the server (IP:PORT) : ");
	std::cin >> c_port;
	printf("\n[!] : Selected Port (PORT:%s)\n", c_port);
	port = std::atoi(c_port);
	CNetwork::Init(port);
	return 0;
}