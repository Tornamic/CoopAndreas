#include "../project_files/stdafx.h"

unsigned int lastSyncTickRate = 0;

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

					CPlayerPed* localPlayer = FindPlayerPed(0);

					int syncRate = 40;
					CVector velocity{};

					if (localPlayer->m_pVehicle)
						velocity = localPlayer->m_pVehicle->m_vecMoveSpeed;
					else
						velocity =  localPlayer->m_vecMoveSpeed;

					if (velocity.x == 0 &&
						velocity.y == 0 &&
						velocity.z == 0)
						syncRate = 100;

					if (GetTickCount() > lastSyncTickRate + syncRate)
					{
						CNetworkVehicleManager::ProcessAll();

						CPackets::PlayerOnFoot* packet = CPacketHandler::PlayerOnFoot__Collect();
						CNetwork::SendPacket(CPacketsID::PLAYER_ONFOOT, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED);

						lastSyncTickRate = GetTickCount();
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
