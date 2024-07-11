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

static CPad* __fastcall CPlayerPed__GetPadFromPlayer_Hook(CPed* This)
{

	if (This == nullptr)
		return nullptr;

	// if ped is local player
	if (This == FindPlayerPed(-1))
	{
	    return &CNetworkPlayerManager::m_pPads[0];
	}

	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(This);
	
	if (player == nullptr)
		return nullptr;

	return &CNetworkPlayerManager::m_pPads[player->m_iPlayerId + 2];
}

CPad* __cdecl CPad__GetPad_Hook(int number)
{
	return &CNetworkPlayerManager::m_pPads[0];
}

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

    CControllerState oldState = pad->NewState;

    pad->NewState = CControllerState();

    if (player->m_lOnFoot != nullptr)
    {
        pad->NewState = player->m_lOnFoot->controllerState;
        player->m_pPed->m_vecMoveSpeed = player->m_lOnFoot->velocity;
        player->m_pPed->m_fCurrentRotation = player->m_lOnFoot->rotation;
        //player->m_pPed->ApplyMoveSpeed();
    }
    
    plugin::CallMethod<0x60EA90, CPlayerPed*>(This);

    CWorld::PlayerInFocus = 0;

    pad->NewState = oldState;
    //printf("not me");
}

void CHook::Init()
{
    Patch_Funcs.push_back([](uint32_t Address) -> bool
    {
        return AdjustBranchPointer(Address, 0x53FB70, CPad__GetPad_Hook, true);
    });
    
    /*Patch_Funcs.push_back([](uint32_t Address) -> bool
    {
        return AdjustBranchPointer(Address, 0x609560, CPlayerPed__GetPadFromPlayer_Hook, true);
    });*/

    patch::SetPointer(0x86D190, CPlayerPed__ProcessControl_Hook);
}
