#include "../project_files/stdafx.h"
int i = 0;
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
				//if (CNetwork::m_bConnected)
				//{
				//	CPackets::PlayerOnFoot* packet;

				//	/*packet->position = CVector(2246.506f, -1259.552f, 23.9531f);
				//	packet->velocity = CVector(0.0f, 0.0f, 0.0f);
				//	packet->rotation = rand() % 360;*/

				//	if (++i >= 10)
				//	{
				//		//FindPlayerPed()->SetPosn(2246.506f, -1259.552f, 23.9531f);
				//		//CNetwork::SendPacket(CPacketsID::PLAYER_ONFOOT, packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
				//		i = 0;
				//	}
				//}
			};
		CCore::Init();
	};
} CoopAndreasPlugin;
