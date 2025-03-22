#include "stdafx.h"
#include "CKeySync.h"
#include "CChat.h"

CControllerState storedOldState{};
CControllerState storedNewState{};
bool storedDisableControls = false;
bool bNightVision;
bool bInfraredVision;
bool storedbCamera = false;
bool storedunk2 = false;
bool storedbPlayerAwaitsInGarage = false;
bool storedbPlayerOnInteriorTransition = false;
bool storedunk3 = false;
bool storedbPlayerSafe = false;
bool storedbPlayerTalksOnPhone = false;
bool storedbPlayerSafeForCutscene = false;
bool storedbPlayerSkipsToDestination = false;
bool storedbDisablePlayerEnterCar = false;
bool storedbDisablePlayerDuck = false;
bool storedbDisablePlayerFireWeapon = false;
bool storedbDisablePlayerFireWeaponWithL1 = false;
bool storedbDisablePlayerCycleWeapon = false;
bool storedbDisablePlayerJump = false;
bool storedbDisablePlayerDisplayVitalStats = false;

void CKeySync::ApplyNetworkPlayerContext(CNetworkPlayer* player)
{
	// store local key state
	CPad* pad = CPad::GetPad(0);
	storedOldState = pad->OldState;
	storedNewState = pad->NewState;

    storedbCamera = pad->unk1;
    storedunk2 = pad->unk2; 
    storedbPlayerAwaitsInGarage = pad->bPlayerAwaitsInGarage; 
    storedbPlayerOnInteriorTransition = pad->bPlayerOnInteriorTransition; 
    storedunk3 = pad->unk3; 
    storedbPlayerSafe = pad->bPlayerSafe; 
    storedbPlayerTalksOnPhone = pad->bPlayerTalksOnPhone; 
    storedbPlayerSafeForCutscene = pad->bPlayerSafeForCutscene; 
    storedbPlayerSkipsToDestination = pad->bPlayerSkipsToDestination; 
    storedbDisablePlayerEnterCar = pad->bDisablePlayerEnterCar; 
    storedbDisablePlayerDuck = pad->bDisablePlayerDuck; 
    storedbDisablePlayerFireWeapon = pad->bDisablePlayerFireWeapon; 
    storedbDisablePlayerFireWeaponWithL1 = pad->bDisablePlayerFireWeaponWithL1; 
    storedbDisablePlayerCycleWeapon = pad->bDisablePlayerCycleWeapon; 
    storedbDisablePlayerJump = pad->bDisablePlayerJump; 
    storedbDisablePlayerDisplayVitalStats = pad->bDisablePlayerDisplayVitalStats;
    
	pad->OldState = player->m_oldControllerState;
	pad->NewState = player->m_newControllerState;

    pad->unk1 = player->m_compressedControllerState.bCamera;
    pad->unk2 = player->m_compressedControllerState.unk2;
    pad->bPlayerAwaitsInGarage = player->m_compressedControllerState.bPlayerAwaitsInGarage;
    pad->bPlayerOnInteriorTransition = player->m_compressedControllerState.bPlayerOnInteriorTransition;
    pad->unk3 = player->m_compressedControllerState.unk3;
    pad->bPlayerSafe = player->m_compressedControllerState.bPlayerSafe;
    pad->bPlayerTalksOnPhone = player->m_compressedControllerState.bPlayerTalksOnPhone;
    pad->bPlayerSafeForCutscene = player->m_compressedControllerState.bPlayerSafeForCutscene;
    pad->bPlayerSkipsToDestination = player->m_compressedControllerState.bPlayerSkipsToDestination;
    pad->bDisablePlayerEnterCar = player->m_compressedControllerState.bDisablePlayerEnterCar;
    pad->bDisablePlayerDuck = player->m_compressedControllerState.bDisablePlayerDuck;
    pad->bDisablePlayerFireWeapon = player->m_compressedControllerState.bDisablePlayerFireWeapon;
    pad->bDisablePlayerFireWeaponWithL1 = player->m_compressedControllerState.bDisablePlayerFireWeaponWithL1;
    pad->bDisablePlayerCycleWeapon = player->m_compressedControllerState.bDisablePlayerCycleWeapon;
    pad->bDisablePlayerJump = player->m_compressedControllerState.bDisablePlayerJump;
    pad->bDisablePlayerDisplayVitalStats = player->m_compressedControllerState.bDisablePlayerDisplayVitalStats;


    bNightVision = patch::GetUChar(0xC402B8, false);
    patch::SetUChar(0xC402B8, false, false);
    bInfraredVision = patch::GetUChar(0xC402B9, false);
    patch::SetUChar(0xC402B9, false, false);
}

void CKeySync::ApplyLocalContext()
{
	// restore local key state
	CPad* pad = CPad::GetPad(0);
	pad->OldState = storedOldState;
	pad->NewState = storedNewState;

    pad->unk1 = storedbCamera;
    pad->unk2 = storedunk2;
    pad->bPlayerAwaitsInGarage = storedbPlayerAwaitsInGarage;
    pad->bPlayerOnInteriorTransition = storedbPlayerOnInteriorTransition;
    pad->unk3 = storedunk3;
    pad->bPlayerSafe = storedbPlayerSafe;
    pad->bPlayerTalksOnPhone = storedbPlayerTalksOnPhone;
    pad->bPlayerSafeForCutscene = storedbPlayerSafeForCutscene;
    pad->bPlayerSkipsToDestination = storedbPlayerSkipsToDestination;
    pad->bDisablePlayerEnterCar = storedbDisablePlayerEnterCar;
    pad->bDisablePlayerDuck = storedbDisablePlayerDuck;
    pad->bDisablePlayerFireWeapon = storedbDisablePlayerFireWeapon;
    pad->bDisablePlayerFireWeaponWithL1 = storedbDisablePlayerFireWeaponWithL1;
    pad->bDisablePlayerCycleWeapon = storedbDisablePlayerCycleWeapon;
    pad->bDisablePlayerJump = storedbDisablePlayerJump;
    pad->bDisablePlayerDisplayVitalStats = storedbDisablePlayerDisplayVitalStats;

    patch::SetUChar(0xC402B8, bNightVision, false);
    patch::SetUChar(0xC402B9, bInfraredVision, false);
}

#define PROCESS_STICK(stick);                                   \
    if (player->m_newControllerState.stick &&                   \
        cState.stick)                                           \
    {                                                           \
        player->m_newControllerState.stick =                    \
            cState.stick;                                       \
        player->m_oldControllerState.stick =                    \
            cState.stick;                                       \
    }                                                           \
    else if (!player->m_newControllerState.stick &&             \
                cState.stick)                                   \
    {                                                           \
        player->m_newControllerState.stick =                    \
            cState.stick;                                       \
        player->m_oldControllerState.stick = 0;                 \
    }                                                           \
    else if (!cState.stick)                                     \
    {                                                           \
        player->m_newControllerState.stick = 0;                 \
        player->m_oldControllerState.stick = 0;                 \
    } 

#define PROCESS_KEY(key);                                       \
    if (cState.key)                                             \
    {                                                           \
        if (player->m_newControllerState.key)                   \
        {                                                       \
            player->m_newControllerState.key = 255;             \
            player->m_oldControllerState.key = 255;             \
        }                                                       \
        else                                                    \
        {                                                       \
            player->m_newControllerState.key = 255;             \
            player->m_oldControllerState.key = 0;               \
        }                                                       \
    }                                                           \
    else                                                        \
    {                                                           \
        player->m_newControllerState.key = 0;                   \
        player->m_oldControllerState.key = 0;                   \
    }
    
void CKeySync::ProcessPlayer(CNetworkPlayer* player)
{
    CCompressedControllerState cState = player->m_compressedControllerState;

    PROCESS_STICK(LeftStickX);
    PROCESS_STICK(LeftStickY);

    PROCESS_KEY(LeftShoulder1);
    PROCESS_KEY(LeftShoulder2);
    PROCESS_KEY(RightShoulder1);
    PROCESS_KEY(RightShoulder2);
    PROCESS_KEY(DPadUp);
    PROCESS_KEY(DPadDown);
    PROCESS_KEY(DPadLeft);
    PROCESS_KEY(DPadRight);
    PROCESS_KEY(Start);
    PROCESS_KEY(Select);
    PROCESS_KEY(ButtonSquare);
    PROCESS_KEY(ButtonTriangle);
    PROCESS_KEY(ButtonCross);
    PROCESS_KEY(ButtonCircle);
    PROCESS_KEY(ShockButtonL);
    PROCESS_KEY(ShockButtonR);
    PROCESS_KEY(m_bChatIndicated);
    PROCESS_KEY(m_bPedWalk);
    PROCESS_KEY(m_bVehicleMouseLook);
    PROCESS_KEY(m_bRadioTrackSkip);
}

#undef PROCESS_KEY
#undef PROCESS_STICK