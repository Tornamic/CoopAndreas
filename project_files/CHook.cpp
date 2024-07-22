#include "stdafx.h"

bool SetBranchPointer(uintptr_t currentAddress, uintptr_t src, injector::memory_pointer dest, bool vp)
{
    auto inst = patch::Get<uint8_t>(currentAddress, false);
    if (inst == 0xE8 || inst == 0xE9)
    {
        uintptr_t funcptr = injector::ReadRelativeOffset(currentAddress + 1, 4, vp).as_int();

        if (funcptr == src)
        {
            injector::MakeRelativeOffset(currentAddress + 1, dest, 4, vp);
            return true;
        }
    }
    return false;
}

static bool AdjustBranchPointer(uintptr_t Address, uintptr_t src, injector::memory_pointer dest, bool vp = true)
{
    return SetBranchPointer(GetGlobalAddress(Address), GetGlobalAddress(src), dest, vp);
}

std::vector<bool(*)(unsigned int)> Patch_Funcs;

CPad* __cdecl CPad__GetPad_Hook(int number)
{
	return &CNetworkPlayerManager::m_pPads[0];
}

bool pPressingDuck[MAX_SERVER_PLAYERS + 2] = {false};

void __fastcall CPlayerPed__ProcessControl_Hook(CPlayerPed* This)
{
    CPlayerPed* localPlayer = FindPlayerPed(0);
     
    if (This == localPlayer)
    {
        plugin::CallMethod<0x60EA90, CPlayerPed*>(This);
        return;
    }

    CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(This);

    if (player == nullptr)
        return;

    CWorld::PlayerInFocus = player->GetInternalId();

    CPad* pad = This->GetPadFromPlayer();

    CControllerState newOldState = pad->NewState;
    CControllerState oldOldState = pad->OldState;

    CCamera oldCameraState = TheCamera;

    if (player->m_lOnFoot != nullptr && player->m_oOnFoot != nullptr)
    {
        TheCamera.m_aCams[0].m_vecFront = player->m_lOnFoot->aimFront;
        TheCamera.m_aCams[0].m_vecSource = player->m_lOnFoot->aimSource;
        TheCamera.m_aCams[0].m_vecSourceBeforeLookBehind = player->m_lOnFoot->aimSourceBeforeLookBehind;
        TheCamera.m_aCams[0].m_vecUp = player->m_lOnFoot->aimUp;

        CUtil::CopyControllerState(pad->OldState, player->m_oOnFoot->controllerState);
        CUtil::CopyControllerState(pad->NewState, player->m_lOnFoot->controllerState);

        if (CUtil::IsDucked(player->m_pPed) != player->m_lOnFoot->ducking) //Forcing crouch sync
        {
            pPressingDuck[CWorld::PlayerInFocus] = true;
            player->m_oShockButtonL = 0;
            player->m_lShockButtonL = 255;
        }
        else if (pPressingDuck[CWorld::PlayerInFocus] == true)
        {
            pPressingDuck[CWorld::PlayerInFocus] = false;
            player->m_oShockButtonL = 255;
            player->m_lShockButtonL = 0;
        }

        pad->OldState.ShockButtonL = player->m_oShockButtonL;
        pad->NewState.ShockButtonL = player->m_lShockButtonL;

        player->m_pPed->m_fHealth = player->m_lOnFoot->health;
        player->m_pPed->m_fArmour = player->m_lOnFoot->armour;

        player->m_pPed->m_fAimingRotation =
            player->m_pPed->m_fCurrentRotation = player->m_lOnFoot->rotation;

        player->m_pPed->m_vecMoveSpeed = player->m_lOnFoot->velocity;
    }
    
    plugin::CallMethod<0x60EA90, CPlayerPed*>(This);

    CWorld::PlayerInFocus = 0;

    pad->NewState = newOldState;
    pad->OldState = oldOldState;

    TheCamera = oldCameraState;
}


void CHook::Init()
{
    Patch_Funcs.push_back([](uint32_t Address) -> bool
    {
        return AdjustBranchPointer(Address, 0x53FB70, CPad__GetPad_Hook, true);
    });

    patch::SetPointer(0x86D190, CPlayerPed__ProcessControl_Hook);
}
