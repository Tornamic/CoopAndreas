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
				if (GetAsyncKeyState(VK_F1))
				{
					CMessages::AddBigMessage("AddBigMessage", 1000, 0);
				}
				if (GetAsyncKeyState(VK_F2))
				{
					CMessages::AddBigMessageQ("AddBigMessage", 1000, 0);
				}
				if (GetAsyncKeyState(VK_F3))
				{
					CMessages::AddBigMessage("AddBigMessage", 1000, 0);
				}
				if (GetAsyncKeyState(VK_F4))
				{
					CMessages::AddBigMessage("AddBigMessage", 1000, 0);
				}

				if (CNetwork::m_bConnected)
				{
					CPackets::PlayerOnFoot* packet = CPacketHandler::PlayerOnFoot__Collect();
					int syncRate = 40;

					if (packet->velocity.x == 0 &&
						packet->velocity.y == 0 &&
						packet->velocity.z == 0)
						syncRate = 100;
					if (GetTickCount() > syncRate + 40)
					{
						lastTickCount = GetTickCount();
						CPackets::PlayerOnFoot::m_last = packet;
						CNetwork::SendPacket(CPacketsID::PLAYER_ONFOOT, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED);
						delete packet;
					}
				}
			};
		Events::drawBlipsEvent += []
			{
				CNetworkPlayerMapPin::Process();
				CNetworkPlayerWaypoint::Process();
			};
		Events::drawingEvent += []
			{
				CNetworkPlayerNameTag::Process();
				CChat::Draw();
			};
		CCore::Init();
		
	};
} CoopAndreasPlugin;
