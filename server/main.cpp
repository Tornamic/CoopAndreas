#include <chrono>
#include <cstdio>
#include <cstring>
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
#include "core/CCommandManager.h"
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

    // #define COOP_SERVER_HOSTING_MODE
#if not defined(COOP_SERVER_HOSTING_MODE)
    char input_line[64*4] = {0};
    char first_input[64] = {0};
    char second_input[64] = {0};
    char third_input[64] = {0};
    char fourth_input[64] = {0};
    CCommandManager::loop_value = true;
    while(CCommandManager::loop_value)
      {
        printf("\n* >> : ");

        if(fgets(input_line, sizeof(input_line), stdin))
          {
             sscanf(input_line, "%s %s %s %s", first_input, second_input, third_input, fourth_input);
             CCommandManager::Init(first_input, second_input, third_input, fourth_input);
          }

        memset(first_input, 0, sizeof(first_input));
        memset(second_input, 0, sizeof(second_input));
        memset(third_input, 0, sizeof(third_input));
        memset(fourth_input, 0, sizeof(fourth_input));
        printf("\n* %s", first_input);
      }
      printf("yoo");
#else 
      printf("\n[!] CoopAndreas Server NO-CLI MODE (NO LAN / SERVER) ");
      CConfigFile svrcfg;
      char ipaddress[DINI_MODULE_MAX_STRING_SIZE];
      int port = 0;
      int maxconnections = 0;
      svrcfg.GeneralFunctionCall(ipaddress, port, maxconnections);
      unsigned short us_port = port;
      CNetwork::Init(ipaddress, us_port, maxconnections);
#endif
        //#endif
    exit(EXIT_SUCCESS); // This Function it will stop any detached threads still runinng
    return 0;
}
