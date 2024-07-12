#include "../project_files/stdafx.h"
int lastTickCount = 0;
CPackets::PlayerOnFoot* previousPacket = NULL;
class CoopAndreas {
public:
    CoopAndreas() {
		Events::shutdownRwEvent += []
			{
				if (CNetwork::m_bConnected)
				{
					enet_peer_disconnect(CNetwork::m_pPeer, 0);
					enet_peer_reset(CNetwork::m_pPeer);
				}
			};
		Events::gameProcessEvent += []
			{
				if (CNetwork::m_bConnected)
				{

					if (GetTickCount() > lastTickCount + 20)
					{
						lastTickCount = GetTickCount();
						CPackets::PlayerOnFoot* packet = CNetwork::CollectOnFootSyncData();
						if (packet != previousPacket)
						{
							CNetwork::SendPacket(CPacketsID::PLAYER_ONFOOT, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED);
							previousPacket = packet;
						}
						delete packet;
					}
				}
			};
		CCore::Init();
	};
} CoopAndreasPlugin;
