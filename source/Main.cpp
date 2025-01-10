#include "stdafx.h"
#include "../project_files/CDXFont.h"
#include <Hooks/WorldHooks.h>
#include "CCutsceneMgr.h"
#include <CDiscordRPCMgr.h>
#include <CCarEnterExit.h>
#include <CTaskSimpleCarSetPedInAsPassenger.h>
#include <CTaskSimpleCarSetPedOut.h>
#include <CNetworkPlayerList.h>
unsigned int lastOnFootSyncTickRate = 0;
unsigned int lastDriverSyncTickRate = 0;
unsigned int lastIdleVehicleSyncTickRate = 0;
unsigned int lastPassengerSyncTickRate = 0;
unsigned int lastPedSyncTickRate = 0;
unsigned int lastWeatherTimeSyncTickRate = 0;
unsigned int lastPlayerAimSyncTickRate = 0;
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
						case ENET_EVENT_TYPE_DISCONNECT:
						{
							CNetwork::Disconnect();
						}
						}
					}
				}
				else if (bBeenConnected && !CNetwork::m_bConnected)
				{
					bBeenConnected = false;
					enet_host_destroy(CNetwork::m_pClient);
					enet_deinitialize();
					CChat::AddMessage("{cecedb}[Network] Disconnected from the server.");
					printf("disconnected from server\n");
				}
			};
		Events::gameProcessEvent += []
			{
				CDiscordRPCMgr::Update();
				CDebugVehicleSpawner::Process();

				if (CNetwork::m_bConnected)
				{
					unsigned int tickCount = GetTickCount();

					CPassengerEnter::Process();

					CPlayerPed* localPlayer = FindPlayerPed(0);

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

					if (!isPassenger && tickCount > (isDriver ? lastDriverSyncTickRate : lastOnFootSyncTickRate) + syncRate)
					{
						if (isDriver)
						{
							CNetworkVehicleManager::UpdateDriver(localPlayer->m_pVehicle);
							lastDriverSyncTickRate = tickCount;
						}
						else
						{
							CPackets::PlayerOnFoot* packet = CPacketHandler::PlayerOnFoot__Collect();
							CNetwork::SendPacket(CPacketsID::PLAYER_ONFOOT, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED);
							delete packet;
							lastOnFootSyncTickRate = tickCount;
						}
					}

					if (isPassenger && tickCount > lastPassengerSyncTickRate + 333)
					{
						CNetworkVehicleManager::UpdatePassenger(localPlayer->m_pVehicle, localPlayer);
						lastPassengerSyncTickRate = tickCount;
					}

					if (tickCount > lastIdleVehicleSyncTickRate + 100)
					{
						CNetworkVehicleManager::UpdateIdle();
						lastIdleVehicleSyncTickRate = tickCount;
					}

					if (tickCount > lastPedSyncTickRate + 40)
					{
						CNetworkPedManager::Update();
						lastPedSyncTickRate = tickCount;
					}

					if (CLocalPlayer::m_bIsHost && tickCount > lastWeatherTimeSyncTickRate + 2000)
					{
						CPacketHandler::GameWeatherTime__Trigger();
						lastWeatherTimeSyncTickRate = tickCount;
					}
					
					// if holds a weapon and is not a driver
					if (localPlayer->m_nActiveWeaponSlot > 0 && !isDriver)
					{
						CControllerState keys = CPad::GetPad(0)->NewState;
						
						// if is player aiming and is not a passenger or doing driveby
						if ((keys.RightShoulder1 && !isPassenger) || CDriveBy::IsPedInDriveby(localPlayer))
						{
							// if is player shooting, update x2 often
							if (tickCount > lastPlayerAimSyncTickRate + keys.ButtonCircle ? 50 : 100)
							{
								CPacketHandler::PlayerAimSync__Trigger();
							}
						}
					}
					else if (localPlayer->m_pVehicle && CUtil::IsVehicleHasTurret(localPlayer->m_pVehicle))
					{
						if (tickCount > lastPlayerAimSyncTickRate + 150)
						{
							CPacketHandler::PlayerAimSync__Trigger();
						}
					}

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
				
				
				CNetworkPlayerList::Draw();

				if (CNetwork::m_bConnected && GetAsyncKeyState(VK_F9))
				{
					char buffer[70];
					sprintf(buffer, "Game/Network: Peds %d/%d Cars %d/%d Recv %d Sent %d", CPools::ms_pPedPool->GetNoOfUsedSpaces(), CNetworkPedManager::m_pPeds.size(), CPools::ms_pVehiclePool->GetNoOfUsedSpaces(), CNetworkVehicleManager::m_pVehicles.size(), CNetwork::m_pClient->totalReceivedPackets, CNetwork::m_pClient->totalSentPackets);
					CDXFont::Draw(100, 10, buffer, D3DCOLOR_ARGB(255, 255, 255, 255));

					for (auto networkVehicle : CNetworkVehicleManager::m_pVehicles)
					{
						if (!networkVehicle || !networkVehicle->m_pVehicle)
							continue;

						CVehicle* vehicle = networkVehicle->m_pVehicle;
						if (!vehicle || !vehicle->m_matrix)
							continue;

						CVector posn = vehicle->m_matrix->pos;
						RwV3d screenCoors; float w, h;
						if (CSprite::CalcScreenCoors({ posn.x, posn.y, posn.z + 1.0f }, &screenCoors, &w, &h, true, true))
						{
							CDXFont::Draw((int)screenCoors.x, (int)screenCoors.y, ("v " + std::to_string(networkVehicle->m_nVehicleId) + "\nS " + std::to_string(networkVehicle->m_bSyncing)).c_str(), D3DCOLOR_ARGB(255, 255, 255, 255));
						}
					}

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
							CDXFont::Draw((int)screenCoors.x, (int)screenCoors.y, ("p " + std::to_string(networkPed->m_nPedId) + "\nS " + std::to_string(networkPed->m_bSyncing)).c_str(), D3DCOLOR_ARGB(255, 255, 255, 255));
						}
					}
				}
			};
		CCore::Init();
		
	};
} CoopAndreasPlugin;