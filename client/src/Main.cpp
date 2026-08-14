#include "CLocalPlayer.h"
#include "CNetworkPlayerManager.h"
#include "CPacketBuffer.h"
#include "CServerTime.h"
#include "stdafx.h"
#include "CCutsceneMgr.h"
#include "UI/CDXFont.h"

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
#include <UI/CNetworkPlayerList.h>
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
#include <CNetworkEntityBlip.h>

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
        Events::shutdownRwEvent += [] {};

        Events::gameProcessEvent.before += []
        {
            ENetEvent event;

            if (CNetwork::m_bConnected)
            {
                bBeenConnected = true;

                CServerTime::Process();
                CNetwork::ProcessReceive();
                GetPacketBuffer().Process();
                CWeatherSync::Process();

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

            if (CNetwork::m_bAuthenticated)
            {
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
                            plugin::CallMethodAndReturn<int8_t, 0x5072A0>(&AudioEngine, i) == 1)
                        {
                            plugin::CallMethod<0x5072B0>(&AudioEngine, i);

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

                if (CLocalPlayer::m_bIsHost)
                {
                    static short lastForced = -999;
                    static short lastNew = -999;
                    static bool lastFlash = false;

                    const bool weatherChanged = CWeather::ForcedWeatherType != lastForced ||
                                                CWeather::NewWeatherType != lastNew ||
                                                CWeather::LightningFlash != lastFlash;

                    const unsigned int interval =
                        (CWeather::LightningBurst || CWeather::LightningFlash || CWeather::Rain > 0.05f) ? 100u : 1500u;

                    if (weatherChanged || tickCount > lastWeatherTimeSyncTickRate + interval)
                    {
                        CWeatherSync::SyncCurrentState();

                        lastWeatherTimeSyncTickRate = tickCount;

                        lastForced = CWeather::ForcedWeatherType;

                        lastNew = CWeather::NewWeatherType;

                        lastFlash = CWeather::LightningFlash;
                    }
                }

                CNetworkVehicleManager::UpdateDamageSync();

                CAimSync::ProcessSyncing();

                CNetworkPedManager::Process();

                CNetworkEntityBlip::Update();

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
        };

        CCore::Init();
    }
} CoopAndreasPlugin;