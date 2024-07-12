#include "../project_files/stdafx.h"
unsigned int lastTickCount = 0;
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
					CPackets::PlayerOnFoot* packet = CPackets::PlayerOnFoot::Collect();
					if (GetTickCount() > lastTickCount + 100 || (CPackets::PlayerOnFoot::m_last == nullptr || !CUtil::CompareControllerStates(CPackets::PlayerOnFoot::m_last->controllerState, packet->controllerState)))
					{
						lastTickCount = GetTickCount();
						CPackets::PlayerOnFoot::m_last = packet;
						CNetwork::SendPacket(CPacketsID::PLAYER_ONFOOT, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED);
						delete packet;
					}
				}
			};
		CCore::Init();
	};
} CoopAndreasPlugin;
