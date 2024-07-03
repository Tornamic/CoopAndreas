#include "../project_files/stdafx.h"

bool created = false;

CNetworkPlayer* player = nullptr;
class CoopAndreas {
public:
    CoopAndreas() {
		CCore::Init();
		Events::gameProcessEvent += []
			{
				// test
				if (KeyPressed(VK_F8) && !created)
				{
					created = true;
					player = new CNetworkPlayer(0, FindPlayerCoors(0));
				}
			};
		Events::initGameEvent += []
			{
				CNetwork::SendPacket(0, "nuke bomb code: 8561214", 24, ENET_PACKET_FLAG_RELIABLE);
			};
	};
} CoopAndreasPlugin;
