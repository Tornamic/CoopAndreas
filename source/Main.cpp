#include "../project_files/stdafx.h"

unsigned int lastOnFootSyncTickRate = 0;
unsigned int lastDriverSyncTickRate = 0;
unsigned int lastIdleVehicleSyncTickRate = 0;
unsigned int lastPassengerSyncTickRate = 0;
unsigned int lastPedSyncTickRate = 0;

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

				if (GetAsyncKeyState(VK_F5))
				{
					CPlayerPed* ped = FindPlayerPed(-1);
					delete ped;
    			}

				if (CNetwork::m_bConnected)
				{
					CPassengerEnter::Process();

					CPlayerPed* localPlayer = FindPlayerPed(0);
					
					if (GetAsyncKeyState(VK_F8))
					{
						Sleep(250);
						CDriveBy::StopDriveby(localPlayer);
					}

					CDriveBy::Process(localPlayer);

					int syncRate = 30;
					CVector velocity{};

					bool isDriver = localPlayer->m_nPedFlags.bInVehicle && localPlayer->m_pVehicle && localPlayer->m_pVehicle->m_pDriver == localPlayer;

					bool isPassenger = localPlayer->m_nPedFlags.bInVehicle && localPlayer->m_pVehicle && localPlayer->m_pVehicle->m_pDriver != localPlayer;

					velocity = isDriver ? localPlayer->m_pVehicle->m_vecMoveSpeed : localPlayer->m_vecMoveSpeed;

					if (velocity.x == 0 && velocity.y == 0 && velocity.z == 0)
					{
						syncRate = 100;
					}

					if (!isPassenger && GetTickCount() > (isDriver ? lastDriverSyncTickRate : lastOnFootSyncTickRate) + syncRate)
					{
						if (isDriver)
						{
							CNetworkVehicleManager::UpdateDriver(localPlayer->m_pVehicle);
							lastDriverSyncTickRate = GetTickCount();
						}
						else
						{
							CPackets::PlayerOnFoot* packet = CPacketHandler::PlayerOnFoot__Collect();
							CNetwork::SendPacket(CPacketsID::PLAYER_ONFOOT, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED);
							delete packet;
							lastOnFootSyncTickRate = GetTickCount();
						}
					}

					if (isPassenger && GetTickCount() > lastPassengerSyncTickRate + 333)
					{
						CNetworkVehicleManager::UpdatePassenger(localPlayer->m_pVehicle, localPlayer);
						lastPassengerSyncTickRate = GetTickCount();
					}

					if (GetTickCount() > lastIdleVehicleSyncTickRate + 100)
					{
						CNetworkVehicleManager::UpdateIdle();
						lastIdleVehicleSyncTickRate = GetTickCount();
					}

					if (GetTickCount() > lastPedSyncTickRate + 100)
					{
						CNetworkPedManager::Update();
						lastPedSyncTickRate = GetTickCount();
					}

					if(!CLocalPlayer::m_bIsHost)
						CNetworkPedManager::Process();
				}
			};
		Events::drawBlipsEvent += []
			{
				CNetworkPlayerMapPin::Process();
				CNetworkPlayerWaypoint::Process();
			};
		Events::drawingEvent += []
			{
				//CDebugPedTasks::Draw();
				CNetworkPlayerNameTag::Process();
				CChat::Draw();
				CChat::DrawInput();
#ifdef _DEV
				if (CNetwork::m_bConnected)
				{
					char buffer[100];
					sprintf(buffer, "Peds %d Recv %d Sent %d", CPools::ms_pPedPool->GetNoOfUsedSpaces(), CNetwork::m_pClient->totalReceivedPackets, CNetwork::m_pClient->totalSentPackets);
					CDXFont::Draw(100, 10, buffer, D3DCOLOR_ARGB(255, 255, 255, 255));
				}
#endif // _DEV

			};
		CCore::Init();
		
	};
} CoopAndreasPlugin;