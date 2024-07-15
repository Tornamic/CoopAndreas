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
    if (This == FindPlayerPed(0))
    {
        plugin::CallMethod<0x60EA90, CPlayerPed*>(This);
        //printf("me");
        return;
    }
    CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(This);

    if (player == nullptr)
        return;

    byte playerNumber = 0;

    for (; playerNumber < MAX_SERVER_PLAYERS + 2; playerNumber++)
    {
        if (This == CWorld::Players[playerNumber].m_pPed)
        {
            break;
        }
    }

    CWorld::PlayerInFocus = playerNumber;

    CPad* pad = This->GetPadFromPlayer();

    CControllerState newOldState = pad->NewState;
    CControllerState oldOldState = pad->OldState;


    if (player->m_lOnFoot != nullptr && player->m_oOnFoot != nullptr)
    {

        if (player->m_lOnFoot->controllerState.ShockButtonL == 255 && pPressingDuck[CWorld::PlayerInFocus] == false && player->m_lOnFoot->isDucked)
        {
            pPressingDuck[CWorld::PlayerInFocus] = true;
            //std::cout << "Ducking: " << player->m_lOnFoot->isDucked << std::endl;
            pad->OldState = player->m_oOnFoot->controllerState;
            pad->NewState = player->m_lOnFoot->controllerState;
        }
        else
        {
            if (player->m_lOnFoot->controllerState.ShockButtonL == 0) //Released the crouch button
            {
                pPressingDuck[CWorld::PlayerInFocus] = false;
            }
            CUtil::CopyExceptShockButtonL(pad->OldState, player->m_oOnFoot->controllerState);
            CUtil::CopyExceptShockButtonL(pad->NewState, player->m_lOnFoot->controllerState);
        }

        player->m_pPed->m_fAimingRotation =
            player->m_pPed->m_fCurrentRotation = player->m_lOnFoot->rotation;
        
        player->m_pPed->m_vecMoveSpeed = player->m_lOnFoot->velocity;

        // log
        
        //player->m_pPed->ApplyMoveSpeed();
    }
    
    plugin::CallMethod<0x60EA90, CPlayerPed*>(This);

    CWorld::PlayerInFocus = 0;

    pad->NewState = newOldState;
    pad->OldState = oldOldState;
}

//CPed* tempPed = nullptr;
//void __declspec(naked) CTaskSimplePlayerOnFoot_MakeAbortable_Hook()
//{
//    _asm mov eax, [esp + 16]
//    _asm pushad
//    _asm mov tempPed, eax
//
//    if (tempPed != FindPlayerPed(0))
//    {
//        _asm popad
//        _asm retn 12
//    }
//    else // it's the local player or keys have already been set.
//    {
//        _asm popad
//        _asm mov edx, 0x6857E0
//        _asm call edx
//        _asm ret
//    }
//}
//
//void __declspec(naked) CTaskSimplePlayerOnFoot_ProcessPed_Hook()
//{
//    _asm mov eax, [esp + 12]
//    _asm pushad
//    _asm mov tempPed, eax
//
//    if (tempPed != FindPlayerPed(0))
//    {
//        _asm popad
//        _asm retn 4
//    }
//    else // it's the local player or keys have already been set.
//    {
//        _asm popad
//        _asm mov edx, 0x688810
//        _asm call edx
//        _asm ret
//    }
//}

void CHook::Init()
{
    Patch_Funcs.push_back([](uint32_t Address) -> bool
    {
        return AdjustBranchPointer(Address, 0x53FB70, CPad__GetPad_Hook, true);
    });

    patch::SetPointer(0x86D190, CPlayerPed__ProcessControl_Hook);

    //patch::SetPointer(0x870904, CTaskSimplePlayerOnFoot_MakeAbortable_Hook);
    //patch::SetPointer(0x870908, CTaskSimplePlayerOnFoot_ProcessPed_Hook);
}
