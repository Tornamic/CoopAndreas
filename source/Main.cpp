#include "../project_files/stdafx.h"

unsigned int lastOnFootSyncTickRate = 0;
unsigned int lastDriverSyncTickRate = 0;
unsigned int lastIdleVehicleSyncTickRate = 0;

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
				CDebugVehicleSpawner::Process();

				if (GetAsyncKeyState(VK_F8) && FindPlayerPed(0)->m_pVehicle != nullptr)
				{
					Sleep(500);
					CStreaming::RequestModel(1050, eStreamingFlags::GAME_REQUIRED);
					CStreaming::LoadAllRequestedModels(false);
					CStreaming::RequestVehicleUpgrade(1050, eStreamingFlags::GAME_REQUIRED);

					plugin::Command<Commands::ADD_VEHICLE_MOD>(CPools::GetVehicleRef(FindPlayerPed(0)->m_pVehicle), 1050);
				}

				if (CNetwork::m_bConnected)
				{

					CPlayerPed* localPlayer = FindPlayerPed(0);

					int syncRate = 40;
					CVector velocity{};

					bool isInVehicle = localPlayer->m_pVehicle && localPlayer->m_pVehicle->m_pDriver == localPlayer;
					velocity = isInVehicle ? localPlayer->m_pVehicle->m_vecMoveSpeed : localPlayer->m_vecMoveSpeed;

					if (velocity.x == 0 && velocity.y == 0 && velocity.z == 0)
					{
						syncRate = 100;
					}

					if (GetTickCount() > (isInVehicle ? lastDriverSyncTickRate : lastOnFootSyncTickRate) + syncRate)
					{
						if (isInVehicle)
						{
							CNetworkVehicleManager::UpdateDriver(localPlayer->m_pVehicle);
							lastDriverSyncTickRate = GetTickCount();
						}
						else
						{
							CPackets::PlayerOnFoot* packet = CPacketHandler::PlayerOnFoot__Collect();
							CNetwork::SendPacket(CPacketsID::PLAYER_ONFOOT, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED);
							lastOnFootSyncTickRate = GetTickCount();
						}
					}

					if (GetTickCount() > lastIdleVehicleSyncTickRate + 250)
					{
						CNetworkVehicleManager::UpdateIdle();
						lastIdleVehicleSyncTickRate = GetTickCount();
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
