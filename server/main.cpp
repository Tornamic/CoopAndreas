
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>

#include "thirdparty-libraries/enet/enet.h"

#include "core/CControllerState.h"
#include "core/NetworkEntityType.h"
#include "core/CPacketListener.h"
#include "core/CVector.h"
#include "core/CNetwork.h"
#include "core/CPed.h"
#include "core/CPedManager.h"
#include "core/CPlayer.h"
#include "core/CPlayerManager.h"
#include "core/CVehicle.h"
#include "core/CVehicleManager.h"
#include "core/VehicleDoorState.h"

//#include "core-external/ConfigDatabase.hpp"



int main(int argc, char *argv[])
{
	//unsigned int configport;
	//ConfigDatabase::Init(configport, "server-config.ini");
	//printf("* variable = %d", configport);
#if defined (_WIN32)
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

	printf("[!] : Version : X.X.X.X, %s %s\n", config, sizeof(void*) == 8 ? "x64" : "x86");
#if defined (_WIN32)
	printf("[!] : Platform : Microsoft Windows \n");
#else
	printf("[!] : Platform : GNU/Linux | BSD \n");
#endif
	CNetwork::Init(6767);
	return 0;
}