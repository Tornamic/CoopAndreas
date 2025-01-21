#include "stdafx.h"
#include "PedHooks.h"
#include "CNetworkPed.h"

static void __cdecl CPopulation__Update_Hook(bool generate)
{
    //if (CLocalPlayer::m_bIsHost)
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
 
    if (CNetwork::m_bConnected && !pPed->IsPlayer())
    {
        _pNetworkPed = CNetworkPedManager::GetPed(pPed);
        if (_pNetworkPed && !_pNetworkPed->m_bSyncing)
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

bool __fastcall CWeapon__Fire_Hook(CWeapon* This, int, CPed* owner, CVector* vecOrigin, CVector* vecEffectPosn, CEntity* targetEntity, CVector* vecTarget, CVector* arg_14)
{
    CNetworkPed* ped = CNetworkPedManager::GetPed(owner);

    if (ped)
    {
        CPackets::PedShotSync packet{};
        packet.pedid = ped->m_nPedId;
        packet.origin = *vecOrigin;
        packet.effect = *vecEffectPosn;
        if (vecTarget)
            packet.target = *vecTarget;
        else if(targetEntity)
            packet.target = targetEntity->GetPosition();

        CNetwork::SendPacket(CPacketsID::PED_SHOT_SYNC, &packet, sizeof packet);

        return This->Fire(owner, vecOrigin, vecEffectPosn, targetEntity, vecTarget, arg_14);
    }
    else
    {
        return This->Fire(owner, vecOrigin, vecEffectPosn, targetEntity, vecTarget, arg_14);
    }

    return false;
}

void CStreaming__RequestSpecialModel_Hook(int modelid, const char* txdName, int flags)
{
    CStreaming::RequestSpecialModel(modelid, txdName, flags);

    if (modelid >= 290 && modelid <= 299)
    {
        char* specialModel = PedHooks::ms_aszLoadedSpecialModels[modelid - 290];

        // copy characters and convert to uppercase
        int i = 0;
        for (; txdName[i] != '\0' && i < 7; i++)
        {
            specialModel[i] = std::toupper(txdName[i]);
        }

        // null-terminate the string
        specialModel[i] = '\0';

        // fill remaining elements with null characters
        for (int j = i + 1; j < 8; j++)
        {
            specialModel[j] = '\0';
        }

        CChat::AddMessage(specialModel);
    }
}

void PedHooks::InjectHooks()
{
    // ped hooks
    patch::RedirectCall(0x53C030, CPopulation__Update_Hook);
    patch::RedirectCall(0x53C054, CPopulation__Update_Hook);
    
    patch::RedirectJump(0x5DEC00, CPed__SetMoveState_Hook);

    patch::RedirectCall(0x61ECCD, CWeapon__Fire_Hook);
    patch::RedirectCall(0x628328, CWeapon__Fire_Hook);
    patch::RedirectCall(0x62B109, CWeapon__Fire_Hook);
    patch::RedirectCall(0x62B12A, CWeapon__Fire_Hook);

    patch::RedirectJump(0x40B45E, CStreaming__RequestSpecialModel_Hook);
}
