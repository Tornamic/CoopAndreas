#include "stdafx.h"
#include "../project_files/CDXFont.h"
#include <Hooks/WorldHooks.h>
#include "CCutsceneMgr.h"
#include <CDiscordRPCMgr.h>
#include <CCarEnterExit.h>
#include <CTaskSimpleCarSetPedInAsPassenger.h>
#include <CTaskSimpleCarSetPedOut.h>
#include <CNetworkPlayerList.h>
#include <CFireManager.h>
#include <semver.h>
#include <COpCodeSync.h>
#include <CNetworkCheckpoint.h>
#include <CEntryExitManager.h>
#include <CEntryExitMarkerSync.h>
#include <CNetworkStaticBlip.h>
unsigned int lastOnFootSyncTickRate = 0;
unsigned int lastDriverSyncTickRate = 0;
unsigned int lastIdleVehicleSyncTickRate = 0;
unsigned int lastPassengerSyncTickRate = 0;
unsigned int lastPedSyncTickRate = 0;
unsigned int lastWeatherTimeSyncTickRate = 0;
unsigned int lastPlayerAimSyncTickRate = 0;
bool bBeenConnected;
bool lastOnMissionFlag;
uint32_t startTime;

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
							CNetwork::ms_nBytesReceivedThisSecondCounter += event.packet->dataLength;
							CNetwork::HandlePacketReceive(event);
							enet_packet_destroy(event.packet); //You should destroy after used it
							break;
						}
						case ENET_EVENT_TYPE_DISCONNECT:
						{
							char buffer[23];
							semver_t expected;
							semver_unpack(event.data, &expected);
							semver_to_string(&expected, buffer, sizeof buffer);
							buffer[22] = '\0';
							CChat::AddMessage("{cecedb}[Network] Version mismatch, server: %s client: %s", buffer, COOPANDREAS_VERSION);
							CNetwork::Disconnect();
						}
						}
					}
					uint32_t currentTime = GetTickCount();
					if (currentTime - startTime >= 1000) 
					{
						CNetwork::ms_nBytesReceivedThisSecond = CNetwork::ms_nBytesReceivedThisSecondCounter;
						CNetwork::ms_nBytesSentThisSecond = CNetwork::ms_nBytesSentThisSecondCounter;
						CNetwork::ms_nBytesReceivedThisSecondCounter = 0;
						CNetwork::ms_nBytesSentThisSecondCounter = 0;
						startTime = currentTime;
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
					// TODO: refactor
					if (COpCodeSync::ms_bLoadingCutscene
						&& !CLocalPlayer::m_bIsHost
						&& CCutsceneMgr::ms_cutsceneName[0]
						&& CCutsceneMgr::ms_cutsceneLoadStatus == 2)
					{
						COpCodeSync::ms_bLoadingCutscene = false;
						Command<Commands::START_CUTSCENE>();
					}

					if (!CLocalPlayer::m_bIsHost)
					{
						for (uint8_t i = 0; i < 4; i++)
						{
							if (COpCodeSync::ms_abLoadingMissionAudio[i]
								&& plugin::CallMethodAndReturn<int8_t, 0x5072A0>(&AudioEngine, i) == 1) // CAudioEngine__GetMissionAudioLoadingStatus
							{
								plugin::CallMethod<0x5072B0>(&AudioEngine, i); // CAudioEngine__PlayLoadedMissionAudio
								COpCodeSync::ms_abLoadingMissionAudio[i] = false;
							}
						}
					}

					if (GetAsyncKeyState(VK_F10) && CLocalPlayer::m_bIsHost)
					{
						CNetworkStaticBlip::Send();
					}

					if (CLocalPlayer::m_bIsHost
						&& CTheScripts::OnAMissionFlag
						&& CTheScripts::ScriptSpace[CTheScripts::OnAMissionFlag] != lastOnMissionFlag)
					{
						lastOnMissionFlag = CTheScripts::ScriptSpace[CTheScripts::OnAMissionFlag];
						CPacketHandler::OnMissionFlagSync__Trigger();
					}

					unsigned int tickCount = GetTickCount();

					CPassengerEnter::Process();

					CPlayerPed* localPlayer = FindPlayerPed(0);

					CDriveBy::Process(localPlayer);
					
					int syncRate = 40;
					CVector velocity{};

					bool isDriver = localPlayer->m_nPedFlags.bInVehicle && localPlayer->m_pVehicle && localPlayer->m_pVehicle->m_pDriver == localPlayer;

					bool isPassenger = localPlayer->m_nPedFlags.bInVehicle && localPlayer->m_pVehicle && localPlayer->m_pVehicle->m_pDriver != localPlayer;

					velocity = isDriver ? localPlayer->m_pVehicle->m_vecMoveSpeed : localPlayer->m_vecMoveSpeed;

					if (velocity.x == 0 && velocity.y == 0 && velocity.z == 0)
					{
						syncRate = 110;
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
							CNetwork::SendPacket(CPacketsID::PLAYER_ONFOOT, packet, sizeof * packet);
							delete packet;
							lastOnFootSyncTickRate = tickCount;
						}
					}

					if (isPassenger && tickCount > lastPassengerSyncTickRate + 333)
					{
						CNetworkVehicleManager::UpdatePassenger(localPlayer->m_pVehicle, localPlayer);
						lastPassengerSyncTickRate = tickCount;
					}

					if (tickCount > lastIdleVehicleSyncTickRate + 150)
					{
						CNetworkVehicleManager::UpdateIdle();
						lastIdleVehicleSyncTickRate = tickCount;
					}

					if (tickCount > lastPedSyncTickRate + 50)
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
							if (tickCount > lastPlayerAimSyncTickRate + (keys.ButtonCircle ? 50 : 100))
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
				if (CEntryExitMarkerSync::ms_bNeedToUpdateAfterProcessingThisFrame 
					&& CLocalPlayer::m_bIsHost
					&& CEntryExitMarkerSync::ms_nLastUpdate + 2000 < GetTickCount())
				{
					CEntryExitMarkerSync::Send();
					CEntryExitMarkerSync::ms_bNeedToUpdateAfterProcessingThisFrame = false;
					CEntryExitMarkerSync::ms_nLastUpdate = GetTickCount();
				}

				if (CNetworkStaticBlip::ms_bNeedToSendAfterThisFrame
					&& CLocalPlayer::m_bIsHost)
				{
					CNetworkStaticBlip::Send();
					CNetworkStaticBlip::ms_bNeedToSendAfterThisFrame = false;
				}

				CNetworkCheckpoint::Process();
				//CDebugPedTasks::Draw();
				CNetworkPlayerNameTag::Process();
				CChat::Draw();
				CChat::DrawInput();
				CNetworkPlayerList::Draw();

				if (CCore::Version.stage != SEMVER_STAGE_RELEASE)
				{
					CDXFont::Draw(0, RsGlobal.maximumHeight - CDXFont::m_fFontSize,
						std::string("CoopAndreas " + std::string(COOPANDREAS_VERSION)).c_str(),
						D3DCOLOR_ARGB(255, 160, 160, 160));
				}

				if (CNetwork::m_bConnected && GetAsyncKeyState(VK_F9))
				{
					char buffer[270];
					sprintf(buffer, "Game/Network: Peds %d/%d | Cars %d/%d | Recv %d %.2f KB/S | Sent %d %.2f KB/S | EnEx %d", CPools::ms_pPedPool->GetNoOfUsedSpaces(), CNetworkPedManager::m_pPeds.size(), CPools::ms_pVehiclePool->GetNoOfUsedSpaces(), CNetworkVehicleManager::m_pVehicles.size(), CNetwork::m_pClient->totalReceivedPackets, CNetwork::ms_nBytesReceivedThisSecond / 1024.0f, CNetwork::m_pClient->totalSentPackets, CNetwork::ms_nBytesSentThisSecond / 1024.0f, CEntryExitManager::mp_poolEntryExits->GetNoOfUsedSpaces());
					CDXFont::Draw(100, 10, buffer, D3DCOLOR_ARGB(255, 255, 255, 255));

					/*for (auto enex : CEntryExitManager::mp_poolEntryExits)
					{
						CVector posn = CVector(enex->m_recEntrance.left, enex->m_recEntrance.bottom, enex->m_fEntranceZ);

						RwV3d screenCoors; float w, h;
						if (CSprite::CalcScreenCoors({ posn.x, posn.y, posn.z + 1.0f }, &screenCoors, &w, &h, true, true))
						{
							if (w >= 10.0f)
							{
								{
									buffer[0] = 0;
									if (enex->m_nFlags.bUnknownInterior)
										sprintf(buffer, "%sbUnknownInterior\n", buffer);
									if (enex->m_nFlags.bUnknownPairing)
										sprintf(buffer, "%sbUnknownPairing\n", buffer);
									if (enex->m_nFlags.bCreateLinkedPair)
										sprintf(buffer, "%sbCreateLinkedPair\n", buffer);
									if (enex->m_nFlags.bRewardInterior)
										sprintf(buffer, "%sbRewardInterior\n", buffer);
									if (enex->m_nFlags.bUsedRewardEntrance)
										sprintf(buffer, "%sbUsedRewardEntrance\n", buffer);
									if (enex->m_nFlags.bCarsAndAircraft)
										sprintf(buffer, "%sbCarsAndAircraft\n", buffer);
									if (enex->m_nFlags.bBikesAndMotorcycles)
										sprintf(buffer, "%sbBikesAndMotorcycles\n", buffer);
									if (enex->m_nFlags.bDisableOnFoot)
										sprintf(buffer, "%sbDisableOnFoot\n", buffer);
									if (enex->m_nFlags.bAcceptNpcGroup)
										sprintf(buffer, "%sbAcceptNpcGroup\n", buffer);
									if (enex->m_nFlags.bFoodDateFlag)
										sprintf(buffer, "%sbFoodDateFlag\n", buffer);
									if (enex->m_nFlags.bUnknownBurglary)
										sprintf(buffer, "%sbUnknownBurglary\n", buffer);
									if (enex->m_nFlags.bDisableExit)
										sprintf(buffer, "%sbDisableExit\n", buffer);
									if (enex->m_nFlags.bBurglaryAccess)
										sprintf(buffer, "%sbBurglaryAccess\n", buffer);
									if (enex->m_nFlags.bEnteredWithoutExit)
										sprintf(buffer, "%sbEnteredWithoutExit\n", buffer);
									if (enex->m_nFlags.bEnableAccess)
										sprintf(buffer, "%sbEnableAccess\n", buffer);
									if (enex->m_nFlags.bDeleteEnex)
										sprintf(buffer, "%sbDeleteEnex\n", buffer);
								}
								sprintf(gString, "name %s\nflags %x\n%s\nleft %f\nbottom %f", enex->m_szName, *(uint16_t*)&enex->m_nFlags, buffer, enex->m_recEntrance.left, enex->m_recEntrance.bottom);
								CDXFont::Draw((int)screenCoors.x, (int)screenCoors.y, gString, D3DCOLOR_ARGB(255, 255, 255, 255));
							}
						}
					}*/
					
					/*for (int i = 0; i < MAX_RADAR_TRACES; i++)
					{
						auto& trace = CRadar::ms_RadarTrace[i];

						if (trace.m_nBlipType != eBlipType::BLIP_CONTACTPOINT
							&& trace.m_nBlipType != eBlipType::BLIP_COORD)
						{
							continue;
						}

						RwV3d screenCoors; float w, h;
						if (CSprite::CalcScreenCoors({ trace.m_vecPos.x, trace.m_vecPos.y, trace.m_vecPos.z + 1.0f }, &screenCoors, &w, &h, false, true))
						{
							if (w >= 0.0f)
							{
							eBlipDisplay::BLIP_DISPLAY_NEITHER;
							char* display = "BLIP_DISPLAY_NEITHER";

							switch (trace.m_nBlipDisplay)
							{
							case eBlipDisplay::BLIP_DISPLAY_MARKER_ONLY:
								display = "BLIP_DISPLAY_MARKER_ONLY";
								break;
							case eBlipDisplay::BLIP_DISPLAY_BOTH:
								display = "BLIP_DISPLAY_BOTH";
								break;
							case eBlipDisplay::BLIP_DISPLAY_BLIP_ONLY:
								display = "BLIP_DISPLAY_BLIP_ONLY";
								break;
							}

							sprintf(gString, "pos %.1f %.1f %.1f\n%s\n%s", 
								trace.m_vecPos.x, trace.m_vecPos.y, trace.m_vecPos.z, 
								trace.m_nBlipType == eBlipType::BLIP_CONTACTPOINT ? "BLIP_CONTACTPOINT" : "BLIP_COORD",
								display);

							CDXFont::Draw((int)screenCoors.x, (int)screenCoors.y, gString, D3DCOLOR_ARGB(255, 255, 255, 255));
							//}
						}
					}*/

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
							CDXFont::Draw((int)screenCoors.x, (int)screenCoors.y, ("v " + std::to_string(networkVehicle->m_nVehicleId) + "\nS " + std::to_string(networkVehicle->m_bSyncing) + "\n" + std::to_string(networkVehicle->m_pVehicle->m_fGasPedal) + " " + std::to_string(networkVehicle->m_pVehicle->m_fBreakPedal)).c_str(), D3DCOLOR_ARGB(255, 255, 255, 255));
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