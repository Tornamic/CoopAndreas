#include "stdafx.h"
#include "PedHooks.h"
#include "CNetworkPed.h"

static void __cdecl CPopulation__Update_Hook(bool generate)
{
    if (CNetwork::m_bConnected)
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

bool __fastcall CWeapon__Fire_Hook(CWeapon* This, SKIP_EDX, CPed* owner, CVector* vecOrigin, CVector* vecEffectPosn, CEntity* targetEntity, CVector* vecTarget, CVector* arg_14)
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
    }
}

int16_t __fastcall CAEPedSpeechAudioEntity__AddSayEvent_Hook(CAEPedSpeechAudioEntity* This, SKIP_EDX, eAudioEvents audioEvent, int16_t gCtx, uint32_t startTimeDelay, float probability, bool overideSilence, bool isForceAudible, bool isFrontEnd)
{
    CPed* ped = (CPed*)((uintptr_t)This - 0x294);

    if (!ped->IsPlayer())
    {
        if (auto networkPed = CNetworkPedManager::GetPed(ped))
        {
            if (!networkPed->m_bSyncing)
            {
                return -1;
            }
        }
    }

    auto result = plugin::CallMethodAndReturn<int16_t, 0x4E6550>(This, audioEvent, gCtx, startTimeDelay, probability, overideSilence, isForceAudible, isFrontEnd);
    
    if (result == -1)
    {
        return result;
    }


    CPackets::PedSay packet{};
    packet.phraseId = gCtx;
    packet.startTimeDelay = startTimeDelay;
    packet.overrideSilence = overideSilence;
    packet.isForceAudible = isForceAudible;
    packet.isFrontEnd = isFrontEnd;

    if (ped == FindPlayerPed(0))
    {
        packet.isPlayer = true;
    }
    else
    {
        if (auto networkPed = CNetworkPedManager::GetPed(ped))
        {
            if (networkPed->m_bSyncing)
            {
                packet.isPlayer = false;
                packet.entityid = networkPed->m_nPedId;
            }
            else
            {
                return result;
            }
        }
        else
        {
            return result;
        }
    }

    CNetwork::SendPacket(CPacketsID::PED_SAY, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);

    return result;
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

    patch::RedirectCall(0x5F000B, CAEPedSpeechAudioEntity__AddSayEvent_Hook);
}
