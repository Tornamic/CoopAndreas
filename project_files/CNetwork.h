#pragma once

#define ENET_IMPLEMENTATION
#include "../enet/enet.h"
#undef ENET_IMPLEMENTATION //Remove the define to prevent other files to load enet library with this define

class CNetwork
{
public:
	static ENetHost* m_pClient;

	static bool Init(const char* ip, int port);
};