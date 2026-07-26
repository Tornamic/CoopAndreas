#include "stdafx.h"
#include "CKeySync.h"
#include "CChat.h"

CControllerState storedOldState{};
CControllerState storedNewState{};

bool bNightVision;
bool bInfraredVision;

uint16_t storedDisablePlayerControls;
char storedDisableKeys[8];

bool bAppliedNetworkCtx = false;

void CKeySync::ApplyNetworkPlayerContext(CNetworkPlayer* player)
{
    assert(!bAppliedNetworkCtx);

    bAppliedNetworkCtx = true;

    // store local key state
    CPad* pad = CPad::GetPad(0);

    storedOldState = pad->OldState;
    storedNewState = pad->NewState;
    storedDisablePlayerControls = pad->DisablePlayerControls;
    memcpy(storedDisableKeys, &pad->bApplyBrakes, 8);

    pad->OldState = player->m_oldControllerState;
    pad->NewState = player->m_newControllerState;
    pad->DisablePlayerControls = 0;
    memset(&pad->bApplyBrakes, 0, 8);

    bNightVision = patch::GetUChar(0xC402B8, false);
    patch::SetUChar(0xC402B8, false, false);
    bInfraredVision = patch::GetUChar(0xC402B9, false);
    patch::SetUChar(0xC402B9, false, false);
}

void CKeySync::ApplyLocalContext()
{
    assert(bAppliedNetworkCtx);

    bAppliedNetworkCtx = false;

    // restore local key state
    CPad* pad = CPad::GetPad(0);

    pad->OldState = storedOldState;
    pad->NewState = storedNewState;
    pad->DisablePlayerControls = storedDisablePlayerControls;
    memcpy(&pad->bApplyBrakes, storedDisableKeys, 8);

    patch::SetUChar(0xC402B8, bNightVision, false);
    patch::SetUChar(0xC402B9, bInfraredVision, false);
}

void CKeySync::CollectState(Packets::Players::SKeySnapshot& keySnapshot)
{
    assert(CWorld::PlayerInFocus == 0);

    CPad* pPad = CPad::GetPad(0);

    keySnapshot.oldControllerState = pPad->OldState;
    keySnapshot.newControllerState = pPad->NewState;

    if (pPad->DisablePlayerControls != 0)
    {
        memset(&keySnapshot.oldControllerState, 0, sizeof(CControllerStateCompressed));
        memset(&keySnapshot.newControllerState, 0, sizeof(CControllerStateCompressed));
        return;
    }

    bool bInVehicle = FindPlayerVehicle(0, false) && FindPlayerPed(0)->m_nPedFlags.bInVehicle;
    bool bOnFoot = !bInVehicle;

    if (pPad->bApplyBrakes && bInVehicle) // force the handbrake
    {
        keySnapshot.oldControllerState.RightShoulder1 = 255;
        keySnapshot.newControllerState.RightShoulder1 = 255;
    }

    if (pPad->bDisablePlayerEnterCar)
    {
        keySnapshot.oldControllerState.ButtonTriangle = 0;
        keySnapshot.newControllerState.ButtonTriangle = 0;
    }

    if (pPad->bDisablePlayerJump && bOnFoot)
    {
        keySnapshot.oldControllerState.ButtonSquare = 0;
        keySnapshot.newControllerState.ButtonSquare = 0;
    }

    if (pPad->bDisablePlayerDuck && bOnFoot)
    {
        keySnapshot.oldControllerState.ShockButtonL = 0;
        keySnapshot.newControllerState.ShockButtonL = 0;
    }

    if (pPad->bDisablePlayerFireWeapon)
    {
        keySnapshot.oldControllerState.ButtonCircle = 0;
        keySnapshot.newControllerState.ButtonCircle = 0;
    }   

    if (pPad->bDisablePlayerFireWeaponWithL1)
    {
        // unused internally
    }

    if (pPad->bDisablePlayerCycleWeapon && bOnFoot)
    {
        keySnapshot.oldControllerState.RightShoulder2 = 0;
        keySnapshot.newControllerState.RightShoulder2 = 0;
        keySnapshot.oldControllerState.LeftShoulder2 = 0;
        keySnapshot.newControllerState.LeftShoulder2 = 0;
    }

    if (pPad->bDisablePlayerJump && bOnFoot)
    {
        keySnapshot.oldControllerState.ButtonSquare = 0;
        keySnapshot.newControllerState.ButtonSquare = 0;
    }

    if (pPad->bDisablePlayerDisplayVitalStats && bOnFoot)
    {
        // useless here
    }
}