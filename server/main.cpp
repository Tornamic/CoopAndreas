
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>

#include "include/enet/enet.h"

#include "Headers/Network/CNetwork.h"

#include "Headers/Server/CLI/Commands/ServerCommandManager.h"

int main(int argc, char *argv[])
{
/*#if defined (_WIN32)
	SetConsoleTitle(L"CoopAndreas Server");
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

	printf("[!] : Version : %s, %s %s\n", COOPANDREAS_VERSION, config, sizeof(void*) == 8 ? "x64" : "x86");
#if defined (_WIN32)
	printf("[!] : Platform : Microsoft Windows \n");
#else
	printf("[!] : Platform : GNU/Linux | BSD \n");
#endif*/
	char array[8][32];
	bool loop = false;
	//CNetwork::Init(6767);
	while(loop == false)
	{
		ServerCommandManager::CommandInput(array);
	}

	return 0;
}