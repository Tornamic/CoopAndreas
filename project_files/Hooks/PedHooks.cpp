#include "stdafx.h"
#include "PedHooks.h"
#include "CNetworkPed.h"

static void __cdecl CPopulation__Update_Hook(bool generate)
{
    if (CLocalPlayer::m_bIsHost)
        CPopulation::Update(generate);
}

CPed* pPed = nullptr;
CNetworkPed* _pNetworkPed = nullptr;
eMoveState nMoveState = (eMoveState)0;
static void __declspec(naked) CPed__SetMoveState_Hook()
{
    __asm
    {
        mov pPed, ecx
        mov eax, [esp+4]
        mov nMoveState, eax
        pushad
    }
 
    if (CNetwork::m_bConnected && !CLocalPlayer::m_bIsHost && !pPed->IsPlayer())
    {
        _pNetworkPed = CNetworkPedManager::GetPed(pPed);
        if (_pNetworkPed)
        {

            pPed->m_nMoveState = _pNetworkPed->m_nMoveState;
            __asm
            {
                popad
                mov eax, 0x5DEC0A
                jmp eax
            }
        }
    }

    pPed->m_nMoveState = nMoveState;

    __asm
    {
        popad
        mov eax, 0x5DEC0A
        jmp eax
    }
}

void PedHooks::InjectHooks()
{
    // ped hooks
    patch::RedirectCall(0x53C030, CPopulation__Update_Hook);
    patch::RedirectCall(0x53C054, CPopulation__Update_Hook);
    
    patch::RedirectJump(0x5DEC00, CPed__SetMoveState_Hook);
}
