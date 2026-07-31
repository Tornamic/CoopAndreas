#include "CLocalPlayer.h"
#include "CNetworkPlayerManager.h"
#include "CPacketBuffer.h"
#include "CServerTime.h"
#include "stdafx.h"
#include "CCutsceneMgr.h"
#include "CDXFont.h"

#include "network/packets/players.h"
#include "network/packet.h"
#include <CCarEnterExit.h>
#include <CDiscordRPCMgr.h>
#include <CEntryExitManager.h>
#include <CEntryExitMarkerSync.h>
#include <CEntryExitTransitionSync.h>
#include <CFireManager.h>
#include <CGeneral.h>
#include <CNetworkAnimQueue.h>
#include <CNetworkCheckpoint.h>
#include <CNetworkPlayerList.h>
#include <CNetworkStaticBlip.h>
#include <COpCodeSync.h>
#include <CPedPlacement.h>
#include <CTaskSimpleCarSetPedInAsPassenger.h>
#include <CTaskSimpleCarSetPedOut.h>
#include <Hooks/WorldHooks.h>
#include <game_sa/CTagManager.h>
#include <semver.h>
#include <CAimSync.h>
#include <CKeySync.h>
#include <CCompatibilityChecker.h>
#include <CWeatherSync.h>
#include <network/packets/scripts.h>

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

class CoopAndreas
{
public:
    CoopAndreas()
    {
        gameShutdownEvent.before += []
        {
            CEntryExitTransitionSync::Shutdown();
        };
        Events::shutdownRwEvent += []
			{
				
			};
        Events::gameProcessEvent.before += []
        {
            ENetEvent event;
            if (CNetwork::m_bConnected)
            {
                bBeenConnected = true;

                CServerTime::Process();
                CNetwork::ProcessReceive();
                GetPacketBuffer().Process();

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
                enet_host_destroy(CNetwork::m_pENetHost);
                enet_deinitialize();
                CChat::AddMessage("{cecedb}[Network] Disconnected from the server.");
            }
        };
        Events::gameProcessEvent += []
        {
            CNetworkAnimQueue::Process();
            CEntryExitTransitionSync::Process();
            CDiscordRPCMgr::Update();
            CDebugVehicleSpawner::Process();

            if (/*CNetwork::m_bConnected*/ CNetwork::m_bAuthenticated)
            {
                // TODO: refactor
                if (COpCodeSync::ms_bLoadingCutscene && !CLocalPlayer::m_bIsHost && CCutsceneMgr::ms_cutsceneName[0] &&
                    CCutsceneMgr::ms_cutsceneLoadStatus == 2)
                {
                    COpCodeSync::ms_bLoadingCutscene = false;
                    Command<Commands::START_CUTSCENE>();
                }

                if (!CLocalPlayer::m_bIsHost)
                {
                    for (uint8_t i = 0; i < 4; i++)
                    {
                        if (COpCodeSync::ms_abLoadingMissionAudio[i] &&
                            plugin::CallMethodAndReturn<int8_t, 0x5072A0>(&AudioEngine, i) ==
                                1)  // CAudioEngine__GetMissionAudioLoadingStatus
                        {
                            plugin::CallMethod<0x5072B0>(&AudioEngine, i);  // CAudioEngine__PlayLoadedMissionAudio
                            COpCodeSync::ms_abLoadingMissionAudio[i] = false;
                        }
                    }
                }

                if (CLocalPlayer::m_bIsHost && CTheScripts::OnAMissionFlag &&
                    static_cast<bool>(CTheScripts::ScriptSpace[CTheScripts::OnAMissionFlag]) != lastOnMissionFlag)
                {
                    lastOnMissionFlag = CTheScripts::ScriptSpace[CTheScripts::OnAMissionFlag];
                        
                    Packets::Scripts::OnMissionFlagSync packet{};
                    packet.bOnMission = CTheScripts::ScriptSpace[CTheScripts::OnAMissionFlag];
                    GetPacketFactory().Send(packet);
                }

                unsigned int tickCount = GetTickCount();

                CPassengerEnter::Process();

                CPlayerPed* localPlayer = FindPlayerPed(0);

                CDriveBy::Process(localPlayer);

                int syncRate = 50;
                CVector velocity{};

                bool isDriver = localPlayer->m_nPedFlags.bInVehicle && localPlayer->m_pVehicle &&
                                localPlayer->m_pVehicle->m_pDriver == localPlayer;

                bool isPassenger = localPlayer->m_nPedFlags.bInVehicle && localPlayer->m_pVehicle &&
                                   localPlayer->m_pVehicle->m_pDriver != localPlayer;

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
                        Packets::Players::OnFootUpdate onFootUpdate{};
                        onFootUpdate.vecPos = localPlayer->GetPosition();
                        onFootUpdate.vecMoveSpeed = localPlayer->m_vecMoveSpeed;
                        onFootUpdate.currentRotation = localPlayer->m_fCurrentRotation;
                        onFootUpdate.aimingRotation = localPlayer->m_fAimingRotation;

                        onFootUpdate.healthSnapshot.iHealth =
                            static_cast<uint8_t>(std::clamp(localPlayer->m_fHealth, 0.0f, 255.0f));
                        onFootUpdate.healthSnapshot.iArmour =
                            static_cast<uint8_t>(std::clamp(localPlayer->m_fArmour, 0.0f, 255.0f));

                        CWeapon& weapon = localPlayer->m_aWeapons[localPlayer->m_nActiveWeaponSlot];
                        onFootUpdate.weaponSnapshot.iWeaponType = weapon.m_eWeaponType;
                        onFootUpdate.weaponSnapshot.iWeaponState = weapon.m_nState;
                        onFootUpdate.weaponSnapshot.nAmmo = weapon.m_nAmmoInClip;

                        onFootUpdate.bDucking = CUtil::IsDucked(localPlayer);
                        onFootUpdate.bHasJetpack = CUtil::IsPedHasJetpack(localPlayer);

                        onFootUpdate.iFightingStyle = localPlayer->m_nFightingStyle;
                        CKeySync::CollectState(onFootUpdate.keySnapshot);

                        GetPacketFactory().Send(onFootUpdate);

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
                    CWeatherSync::SyncCurrentState();
                    lastWeatherTimeSyncTickRate = tickCount;
                }
                CNetworkVehicleManager::UpdateDamageSync();

                CAimSync::ProcessSyncing();

                CNetworkPedManager::Process();

                static uint32_t crashHotkeyPressedAt = 0;

                if (GetAsyncKeyState(VK_F7) && GetAsyncKeyState(VK_F10) && GetAsyncKeyState(VK_NUMPAD1))
                {
                    if (crashHotkeyPressedAt == 0)
                    {
                        crashHotkeyPressedAt = tickCount;
                    }
                    if (tickCount - crashHotkeyPressedAt >= 5000)
                    {
                        *(uint*)0x0 = 0xDEADC0DE;
                    }
                }
                else
                {
                    crashHotkeyPressedAt = 0;
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
            if (CEntryExitMarkerSync::ms_bUpdateAfterProcessingThisFrame && CLocalPlayer::m_bIsHost &&
                CEntryExitMarkerSync::ms_nLastUpdate + 5000 < GetTickCount())
            {
                CEntryExitMarkerSync::Send();
                CEntryExitMarkerSync::ms_bUpdateAfterProcessingThisFrame = false;
                CEntryExitMarkerSync::ms_nLastUpdate = GetTickCount();
            }

            if (CNetworkStaticBlip::ms_bNeedToSendAfterThisFrame && CLocalPlayer::m_bIsHost)
            {
                CNetworkStaticBlip::Send();
                CNetworkStaticBlip::ms_bNeedToSendAfterThisFrame = false;
            }

            /*if (GetAsyncKeyState(VK_F11) && CLocalPlayer::m_bIsHost)
            {
                CEntryExitMarkerSync::Send();
            }*/

            CNetworkCheckpoint::Process();
            CNetworkPlayerNameTag::Process();
            CChat::Draw();
            CChat::DrawInput();

            if (FrontEndMenuManager.m_bPrefsShowHud)
            {
                CNetworkPlayerList::Draw();
            }

            if (FrontEndMenuManager.m_bPrefsShowHud && CCore::Version.stage != SEMVER_STAGE_RELEASE)
            {
                CDXFont::Draw(0, RsGlobal.maximumHeight - CDXFont::m_fFontSize,
                    std::string("CoopAndreas " + std::string(COOPANDREAS_VERSION)).c_str(),
                    D3DCOLOR_ARGB(255, 160, 160, 160));
            }

            if (/*CNetwork::m_bConnected && */ GetAsyncKeyState(VK_F10))
            {
                CDebugPedTasks::Draw();
            }

            if (/*CNetwork::m_bConnected && */ GetAsyncKeyState(VK_F9))
            {
                char buffer[270];
                sprintf(buffer,
                    "IsHost=%d | RTT %dms | Jitter %dms | Game/Network: Peds %d/%d | Cars %d/%d | Recv %d %.2f KB/S | Sent %d %.2f KB/S | EnEx %d",
                    CLocalPlayer::m_bIsHost, CNetwork::m_pPeer->roundTripTime, CNetwork::m_pPeer->roundTripTimeVariance, CPools::ms_pPedPool->GetNoOfUsedSpaces(),
                    CNetworkPedManager::m_pPeds.size(), CPools::ms_pVehiclePool->GetNoOfUsedSpaces(),
                    CNetworkVehicleManager::m_pVehicles.size(), CNetwork::m_pENetHost->totalReceivedPackets,
                    CNetwork::ms_nBytesReceivedThisSecond / 1024.0f, CNetwork::m_pENetHost->totalSentPackets,
                    CNetwork::ms_nBytesSentThisSecond / 1024.0f,
                    CEntryExitManager::mp_poolEntryExits->GetNoOfUsedSpaces());
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
                            sprintf(gString, "name %s\nflags %x\n%s\nleft %f\nbottom %f\narea %d\nlink %x\nlast %d",
                enex->m_szName, *(uint16_t*)&enex->m_nFlags, buffer, enex->m_recEntrance.left,
                enex->m_recEntrance.bottom, enex->m_nArea, enex->m_pLink,
                CEntryExitMarkerSync::ms_mapLastEnExUpdate[enex]); CDXFont::Draw((int)screenCoors.x, (int)screenCoors.y,
                gString, D3DCOLOR_ARGB(255, 255, 255, 255));
                        }
                    }

                }*/

                /*for (auto tag : CTagManager::ms_tagDesc)
                {
                    if (!tag.m_pEntity)
                        continue;
                    if (tag.m_nAlpha != 0)
                        continue;

                    CVector& posn = tag.m_pEntity->GetPosition();

                    RwV3d screenCoors;
                    float w, h;
                    if (CSprite::CalcScreenCoors({posn.x, posn.y, posn.z + 1.0f}, &screenCoors, &w, &h, true, true))
                    {
                        sprintf(gString, "alpha %d\n%f %f %f", tag.m_nAlpha, posn.x, posn.y, posn.z);
                        CDXFont::Draw(
                            (int)screenCoors.x, (int)screenCoors.y, gString, D3DCOLOR_ARGB(255, 255, 255, 255));
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
                    if (CSprite::CalcScreenCoors({ trace.m_vecPos.x, trace.m_vecPos.y, trace.m_vecPos.z + 1.0f },
                &screenCoors, &w, &h, false, true))
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

                        CDXFont::Draw((int)screenCoors.x, (int)screenCoors.y, gString, D3DCOLOR_ARGB(255, 255, 255,
                255));
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
                    RwV3d screenCoors;
                    float w, h;
                    if (CSprite::CalcScreenCoors({posn.x, posn.y, posn.z + 1.0f}, &screenCoors, &w, &h, true, true))
                    {
                        CDXFont::Draw((int)screenCoors.x, (int)screenCoors.y,
                            ("v " + std::to_string(networkVehicle->m_nVehicleId) + "\nS " +
                                std::to_string(networkVehicle->m_bSyncing) + "\n" +
                                std::to_string(networkVehicle->m_pVehicle->m_fGasPedal) + " " +
                                std::to_string(networkVehicle->m_pVehicle->m_fBreakPedal) + "\nC " +
                                std::to_string(networkVehicle->m_pVehicle->m_nCreatedBy) + "\npaint " + std::to_string(networkVehicle->m_pVehicle->GetRemapIndex())
                                
                                )
                                .c_str(),
                            D3DCOLOR_ARGB(255, 255, 255, 255));
                        
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
                    RwV3d screenCoors;
                    float w, h;
                    if (CSprite::CalcScreenCoors({posn.x, posn.y, posn.z + 1.0f}, &screenCoors, &w, &h, true, true))
                    {
                        CDXFont::Draw((int)screenCoors.x, (int)screenCoors.y,
                            ("p " + std::to_string(networkPed->m_nPedId) + "\nS " +
                                std::to_string(networkPed->m_bSyncing) + "\n" + std::to_string(ped->m_nMoveState))
                                .c_str(),
                            D3DCOLOR_ARGB(255, 255, 255, 255));
                    }
                }
                

                std::string buf;
                for (int i = 0; i < 180; i++)
                {
                    CAnimBlock& block = CAnimManager::ms_aAnimBlocks[i];
                    if (!block.bLoaded)
                        continue;

                    buf += std::string(block.szName) + '\n';
                }

                CDXFont::Draw(10, 25, buf.c_str(), D3DCOLOR_ARGB(255, 255, 255, 255));
                
                CVector posn = FindPlayerPed(0)->m_matrix->pos;
                RwV3d screenCoors;
                float w, h;
                //if (CSprite::CalcScreenCoors({posn.x, posn.y, posn.z + 1.0f}, &screenCoors, &w, &h, true, true))
                {
                    float orientsim = atan2f(TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecFront.x, TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecFront.y);
                    char buf[512];
                    sprintf(buf, "cam %f/%f\ncur %f des %f\n%s/%s\nfront %f %f %f\nsource %f %f %f\nup %f %f %f", TheCamera.m_fOrientation, orientsim,
                        FindPlayerPed(0)->m_fCurrentRotation, FindPlayerPed(0)->m_fAimingRotation, CamModeToString(TheCamera.m_aCams[0].m_nMode), CamModeToString((eCamMode)TheCamera.m_PlayerWeaponMode.m_nMode),
                        TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecFront.x,
                        TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecFront.y,
                        TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecFront.z,
                        TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecSource.x,
                        TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecSource.y,
                        TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecSource.z,
                        TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecUp.x,
                        TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecUp.y,
                        TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecUp.z
                        );
                    CDXFont::Draw((int)100, (int)100, buf, D3DCOLOR_ARGB(255, 255, 255, 255));
                    
                    /*CVehicle* pVehicle = FindPlayerPed(0)->m_pVehicle;
                    if (pVehicle)
                    {
                        static float maxaaa = 0.0f;
                        maxaaa = std::max(maxaaa, std::abs(pVehicle->m_vecTurnSpeed.x));
                        maxaaa = std::max(maxaaa, std::abs(pVehicle->m_vecTurnSpeed.y));
                        maxaaa = std::max(maxaaa, std::abs(pVehicle->m_vecTurnSpeed.z));
                        sprintf(buf, 
                            "pos {%f, %f, %f}\n"
                            "right {%f, %f, %f}\n"
                            "up {%f, %f, %f}\n"
                            "moveSpeed {%f, %f, %f}\n"
                            "turnSpeed {%f, %f, %f} (%f)"
                            , 
                            pVehicle->m_matrix->pos.x, pVehicle->m_matrix->pos.y, pVehicle->m_matrix->pos.z,
                            pVehicle->m_matrix->right.x, pVehicle->m_matrix->right.y, pVehicle->m_matrix->right.z,
                            pVehicle->m_matrix->up.x, pVehicle->m_matrix->up.y, pVehicle->m_matrix->up.z,
                            pVehicle->m_vecMoveSpeed.x, pVehicle->m_vecMoveSpeed.y, pVehicle->m_vecMoveSpeed.z,
                            pVehicle->m_vecTurnSpeed.x, pVehicle->m_vecTurnSpeed.y, pVehicle->m_vecTurnSpeed.z, maxaaa
                        );
                        CDXFont::Draw((int)350, (int)100, buf, D3DCOLOR_ARGB(255, 255, 255, 255));
                    }*/
                }

                /*
                for (auto networkPlayer : CNetworkPlayerManager::m_pPlayers)
                {
                    posn = networkPlayer->m_pPed->m_matrix->pos;
                    if (CSprite::CalcScreenCoors({posn.x, posn.y, posn.z + 1.0f}, &screenCoors, &w, &h, true, true))
                    {
                        char buf[128];
                        sprintf(buf, "cam %f\nncur %f ndes %f\ncur %f des %f",
                            networkPlayer->m_cameraSnapshot.orientation.m_angle,
                            networkPlayer->m_onFootSnapshotInterpolated.currentRotation.m_angle,
                            networkPlayer->m_onFootSnapshotInterpolated.aimingRotation.m_angle,
                            networkPlayer->m_pPed->m_fCurrentRotation, networkPlayer->m_pPed->m_fAimingRotation);

                        CDXFont::Draw((int)screenCoors.x, (int)screenCoors.y, buf, D3DCOLOR_ARGB(255, 255, 255, 255));
                    }
                }
                */
            }
        };
        CCore::Init();
    };
} CoopAndreasPlugin;
