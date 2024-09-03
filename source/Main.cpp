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

				if (GetAsyncKeyState(VK_F8) && FindPlayerPed(0)->m_pVehicle)
				{
					Sleep(300);
					CStreaming::RequestModel(1010, eStreamingFlags::GAME_REQUIRED);
					CStreaming::LoadAllRequestedModels(false);
					CStreaming::RequestVehicleUpgrade(1010, eStreamingFlags::GAME_REQUIRED);

					char count = 10;
					while (!CStreaming::HasVehicleUpgradeLoaded(1010) && count)
					{
						Sleep(5);
						count--;
					}

					plugin::Command<Commands::ADD_VEHICLE_MOD>(CPools::GetVehicleRef(FindPlayerPed(0)->m_pVehicle), 1010);
				}

				if (CNetwork::m_bConnected)
				{
					CPassengerEnter::Process();

					CPlayerPed* localPlayer = FindPlayerPed(0);

					int syncRate = 30;
					CVector velocity{};

					bool isInVehicle = localPlayer->m_pVehicle && localPlayer->m_pVehicle->m_pDriver == localPlayer;
					velocity = isInVehicle ? localPlayer->m_pVehicle->m_vecMoveSpeed : localPlayer->m_vecMoveSpeed;

					if (velocity.x == 0 && velocity.y == 0 && velocity.z == 0)
					{
						syncRate = 60;
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

					if (GetTickCount() > lastIdleVehicleSyncTickRate + 100)
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
