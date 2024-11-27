#include "stdafx.h"
#include "../project_files/CDXFont.h"
#include <Hooks/WorldHooks.h>
#include "CCutsceneMgr.h"
unsigned int lastOnFootSyncTickRate = 0;
unsigned int lastDriverSyncTickRate = 0;
unsigned int lastIdleVehicleSyncTickRate = 0;
unsigned int lastPassengerSyncTickRate = 0;
unsigned int lastPedSyncTickRate = 0;
bool bBeenConnected;
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
		Events::gameProcessEvent.before += []
			{
				ENetEvent event;
				if (CNetwork::m_bConnected)
				{
					bBeenConnected = true;
					while (enet_host_service(CNetwork::m_pClient, &event, 1) != 0)
					{
						switch (event.type)
						{
						case ENET_EVENT_TYPE_RECEIVE:
						{
							CNetwork::HandlePacketReceive(event);
							enet_packet_destroy(event.packet); //You should destroy after used it
							break;
						}
						}
					}
				}
				else if (bBeenConnected && !CNetwork::m_bConnected)
				{
					// disconnect
					enet_host_destroy(CNetwork::m_pClient);
					enet_deinitialize();
					printf("disconnected from server");
				}
			};
		Events::gameProcessEvent += []
			{
				CDebugVehicleSpawner::Process();

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

					if (GetTickCount() > lastPedSyncTickRate + 40)
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
					char buffer[70];
					sprintf(buffer, "Game/Network: Peds %d/%d Cars %d/%d Recv %d Sent %d", CPools::ms_pPedPool->GetNoOfUsedSpaces(), CNetworkPedManager::m_pPeds.size(), CPools::ms_pVehiclePool->GetNoOfUsedSpaces(), CNetworkVehicleManager::m_pVehicles.size(), CNetwork::m_pClient->totalReceivedPackets, CNetwork::m_pClient->totalSentPackets);
					CDXFont::Draw(100, 10, buffer, D3DCOLOR_ARGB(255, 255, 255, 255));

					for (auto networkPed : CNetworkPedManager::m_pPeds)
					{
						if (!networkPed || !networkPed->m_pPed)
							continue;

						CPed* ped = networkPed->m_pPed;
						if (!ped || !ped->m_matrix)
							continue;

						CVector posn = ped->m_matrix->pos;
						RwV3d screenCoors; float w, h;
						if (CSprite::CalcScreenCoors({ posn.x, posn.y, posn.z + 1.0f }, &screenCoors, &w, &h, true, true))
						{
							CDXFont::Draw((int)screenCoors.x, (int)screenCoors.y, std::to_string(networkPed->m_nPedId).c_str(), D3DCOLOR_ARGB(255, 255, 255, 255));
						}
					}
				}
#endif // _DEV

			};
		CCore::Init();
		
	};
} CoopAndreasPlugin;