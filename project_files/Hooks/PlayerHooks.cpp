#include "stdafx.h"
#include "PlayerHooks.h"
#include "CKeySync.h"
#include "CAimSync.h"
#include <game_sa/CPedDamageResponseInfo.h>

static void __fastcall CPlayerPed__ProcessControl_Hook(CPlayerPed* This)
{
    CPlayerPed* localPlayer = FindPlayerPed(0);

    if (This == localPlayer)
    {
        patch::SetRaw(0x6884C4, "\xD9\x96\x5C\x05\x00\x00", 6, false);
        plugin::CallMethod<0x60EA90, CPlayerPed*>(This);
        patch::Nop(0x6884C4, 6, false);
        return;
    }

    CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(This);

    if (player == nullptr)
    {
        plugin::CallMethod<0x60EA90, CPlayerPed*>(This);
        return;
    }

    int playerNum = player->GetInternalId();

    if (playerNum == -1)
    {
        plugin::CallMethod<0x60EA90, CPlayerPed*>(This);
        return;
    }

    CWorld::PlayerInFocus = playerNum;

    CKeySync::ApplyNetworkPlayerContext(player);
    CAimSync::ApplyNetworkPlayerContext(player);
    //CStatsSync::ApplyNetworkPlayerContext(player);

    //if (CPad::GetPad(0)->NewState.RightShoulder1) // is aiming
    //{
    //    player->m_pPed->m_fCurrentRotation = player->m_lOnFoot->currentRotation;
    //}
    //player->m_pPed->m_fAimingRotation = player->m_lOnFoot->aimingRotation;

    player->m_pPed->m_fHealth = player->m_lOnFoot->health;
    player->m_pPed->m_fArmour = player->m_lOnFoot->armour;

    CTask* activeTask = player->m_pPed->m_pIntelligence->m_TaskMgr.GetActiveTask();

    if (activeTask && activeTask->GetId() != eTaskType::TASK_COMPLEX_JUMP)
    {
        player->m_pPed->m_vecMoveSpeed = player->m_lOnFoot->velocity;
    }

    plugin::CallMethod<0x60EA90, CPlayerPed*>(This);

    //player->m_pPed->m_fAimingRotation = player->m_lOnFoot->aimingRotation;

    CWorld::PlayerInFocus = 0;

    CKeySync::ApplyLocalContext();
    CAimSync::ApplyLocalContext();
    //CStatsSync::ApplyLocalContext();
}

static void __fastcall CWeapon__DoBulletImpact_Hook(CWeapon* weapon, int padding, CEntity* owner, CEntity* victim, CVector* startPoint, CVector* endPoint, CColPoint* colPoint, int incrementalHit)
{
    if (owner == FindPlayerPed(0) && victim)
    {
        CPackets::PlayerBulletShot* packet = new CPackets::PlayerBulletShot;
        packet->entityType = victim->m_nType + 1;

        packet->targetid = -1;

        if (victim != nullptr)
        {
            switch (victim->m_nType)
            {
            case eEntityType::ENTITY_TYPE_PED: // ped or player
            {
                if (auto playerTarget = CNetworkPlayerManager::GetPlayer(victim))
                {
                    packet->targetid = playerTarget->m_iPlayerId;
                    packet->entityType = eNetworkEntityType::NETWORK_ENTITY_TYPE_PLAYER;
                }
                else if (auto pedTarget = CNetworkPedManager::GetPed(victim))
                {
                    packet->targetid = pedTarget->m_nPedId;
                }
                break;
            }
            case eEntityType::ENTITY_TYPE_VEHICLE:
            {
                if (auto vehicleTarget = CNetworkVehicleManager::GetVehicle(victim))
                    packet->targetid = vehicleTarget->m_nVehicleId;
                break;
            }
            }
        }

        packet->startPos = *startPoint;
        packet->endPos = *endPoint;
        packet->colPoint = *colPoint;
        packet->incrementalHit = incrementalHit;

        CNetwork::SendPacket(CPacketsID::PLAYER_BULLET_SHOT, packet, sizeof * packet);

        weapon->DoBulletImpact(owner, victim, startPoint, endPoint, colPoint, incrementalHit);

        return;
    }
    else if (owner->m_nType == eEntityType::ENTITY_TYPE_PED)
    {
        CPed* ped = (CPed*)owner;

        if (ped->m_nPedType > PED_TYPE_PLAYER1)
        {
            weapon->DoBulletImpact(owner, victim, startPoint, endPoint, colPoint, incrementalHit);
        }
    }
}

static void __fastcall CPedIK__PointGunInDirection_Hook(CPedIK* This, int padding, float dirX, float dirY, char flag, float float1)
{
    //if (This->m_pPed == FindPlayerPed(0))
    //{
    //    // save last aim data for syncing
    //    CLocalPlayer::m_vecLastAimX = dirX;
    //    CLocalPlayer::m_vecLastAimY = dirY;

    //    This->PointGunInDirection(dirX, dirY, flag, float1);
    //    return;
    //}

    //CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(This->m_pPed);

    //if (player == nullptr)
    //{
    //    This->PointGunInDirection(dirX, dirY, flag, float1);
    //    return;
    //}

    //if (player->m_lOnFoot == nullptr)
    //    return;

    //player->m_pPed->m_fAimingRotation = player->m_lOnFoot->aimingRotation;
    //
    //eWeaponType weapon = player->m_pPed->m_aWeapons[player->m_pPed->m_nActiveWeaponSlot].m_eWeaponType;

    ///*if (weapon != WEAPON_SNIPERRIFLE)
    //    dirY = player->m_aimSyncData.aimY;*/

    This->PointGunInDirection(dirX, dirY, flag, float1);
}

static void __fastcall CPlayerPed__dctor_Hook(CPlayerPed* This, int)
{
    This->m_pPlayerData->m_nMeleeWeaponAnimReferencedExtra = 0;
    plugin::CallMethod<0x6093B0, CPlayerPed*>(This);
}

void CReferences__RemoveReferencesToPlayer_Hook()
{
    plugin::Call<0x571AD0>(); // CReferences::RemoveReferencesToPlayer();
    
    if (CNetwork::m_bConnected)
    {
        CPackets::RespawnPlayer packet{};
        CNetwork::SendPacket(CPacketsID::RESPAWN_PLAYER, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    }
}

bool __fastcall CWeapon__TakePhotograph_Hook(CWeapon* This, int, CEntity* entity, CVector* point)
{
    if (entity == FindPlayerPed(0))
    {
        return This->TakePhotograph(entity, point);
    }

    return false;
}

void __fastcall CTaskSimpleJetPack__DropJetPack_Hook(CTaskSimpleJetPack* This, int, CPed* ped)
{
    if (ped != FindPlayerPed(0) && ped->IsPlayer())
    {
        // dont create a pickup if the player is network
        This->m_bIsFinished = true;
    }

    plugin::CallMethod<0x67B660, CTaskSimpleJetPack*>(This, ped); // CTaskSimpleJetPack::DropJetPack
}

void __fastcall CPedDamageResponseCalculator__ComputeWillKillPed_Hook(uintptr_t This, int, CPed* ped, CPedDamageResponseInfo* dmgResponse, bool speak)
{
    plugin::CallMethod<0x4B3210, uintptr_t>(This, ped, dmgResponse, speak);

    if (ped == FindPlayerPed(0))
    {
        return;
    }

    if (ped->m_nPedType > 3) // peds
    {
        if (auto networkPed = CNetworkPedManager::GetPed(ped))
        {
            if (!networkPed->m_bSyncing)
            {
                if (networkPed->m_fHealth >= 1.0f)
                {
                    ped->m_fHealth = networkPed->m_fHealth;
                    dmgResponse->m_bHealthZero = false;
                    dmgResponse->m_bForceDeath = false;
                    dmgResponse->m_fDamageHealth = 0.0f;
                    dmgResponse->m_fDamageArmor = 0.0f;
                }
            }
        }

        return;
    }

    // players
    if (auto networkPlayer = CNetworkPlayerManager::GetPlayer(ped))
    {
        if (networkPlayer->m_lOnFoot->health >= 1.0f)
        {
            ped->m_fHealth = networkPlayer->m_lOnFoot->health;
            dmgResponse->m_bHealthZero = false;
            dmgResponse->m_bForceDeath = false;
            dmgResponse->m_fDamageHealth = 0.0f;
            dmgResponse->m_fDamageArmor = 0.0f;
        }
    }
}

bool __fastcall CPlayerPed__CanPlayerStartMission_Hook(CPlayerPed* This, int)
{
    return This->CanPlayerStartMission()/* && CLocalPlayer::m_bIsHost*/;
}

void __fastcall CPlayerPed__ProcessWeaponSwitch_Hook(CPlayerPed* This, int, CPad* pad)
{
    if (CWorld::PlayerInFocus == 0)
    {
        This->ProcessWeaponSwitch(pad);
    }
}

void __fastcall CTaskSimplePlayerOnFoot__PlayIdleAnimations_Hook(CTaskSimplePlayerOnFoot* This, int, CPlayerPed* playerPed)
{
    if (CWorld::PlayerInFocus == 0)
    {
        plugin::CallMethod<0x6872C0>(This, playerPed);
    }
}

bool __fastcall CPad__JumpJustDown_Hook(CPad* This)
{
    if (CWorld::PlayerInFocus == 0)
    {
        return This->JumpJustDown();
    }

    return false;
}

void __fastcall CTaskComplexJump_CTaskManager__SetTask_Hook(CTaskManager* This, int, CTask* task, int tasksId, bool a4)
{
    This->SetTask(task, tasksId, a4);

    if (CWorld::PlayerInFocus == 0)
    {
        CLocalPlayer::BuildTaskPacket(TASK_COMPLEX_JUMP);
    }
}

void __fastcall CRunningScript__DoDeatharrestCheck_Hook(CRunningScript* This, int)
{
    if (!This->m_bWastedBustedCheck) 
    {
        return;
    }

    if (!CTheScripts::IsPlayerOnAMission()) 
    {
        return;
    }

    bool wastedOrBusted = false;
    
    auto* playerInfo = &CWorld::Players[CWorld::PlayerInFocus];

    if (playerInfo->m_nPlayerState == ePlayerState::PLAYERSTATE_HASDIED ||
        playerInfo->m_nPlayerState == ePlayerState::PLAYERSTATE_HASBEENARRESTED)
    {
        wastedOrBusted = true;
    }

    if (!wastedOrBusted)
    {
        for (auto networkPlayer : CNetworkPlayerManager::m_pPlayers)
        {
            if (auto ped = networkPlayer->m_pPed)
            {
                if (ped->m_ePedState == PEDSTATE_ARRESTED ||
                    ped->m_ePedState == PEDSTATE_DEAD ||
                    (ped->m_ePedState == PEDSTATE_DIE && ped->m_nPedFlags.bIsDyingStuck))
                {
                    wastedOrBusted = true;
                }
            }
        }
    }

    if (wastedOrBusted)
    {
        CMessages::ClearSmallMessagesOnly();
        memset(&CTheScripts::ScriptSpace[CTheScripts::OnAMissionFlag], 0, sizeof(uint32_t));
        
        if (This->m_nSP > 1u)
        {
            uint16_t nsp = This->m_nSP;
            do
            {
                --nsp;
            } while (nsp > 1u);
            This->m_nSP = nsp;
        }
        This->m_pCurrentIP = This->m_apStack[--This->m_nSP];

        This->m_bWastedOrBusted = true;
        This->m_nWakeTime = 0;
    }
}

void PlayerHooks::InjectHooks()
{
    patch::SetPointer(0x86D190, CPlayerPed__ProcessControl_Hook);

    patch::RedirectCall(0x73CD92, CWeapon__DoBulletImpact_Hook);
    patch::RedirectCall(0x741185, CWeapon__DoBulletImpact_Hook);
    patch::RedirectCall(0x741199, CWeapon__DoBulletImpact_Hook);
    patch::RedirectCall(0x7411DF, CWeapon__DoBulletImpact_Hook);
    patch::RedirectCall(0x7412DF, CWeapon__DoBulletImpact_Hook);
    patch::RedirectCall(0x741E30, CWeapon__DoBulletImpact_Hook);

    patch::RedirectCall(0x5FDF7A, CPedIK__PointGunInDirection_Hook);
    patch::RedirectCall(0x61F351, CPedIK__PointGunInDirection_Hook);
    patch::RedirectCall(0x62876D, CPedIK__PointGunInDirection_Hook);

    // fix CPlayerPed dctor crash
    patch::RedirectCall(0x60A9A3, CPlayerPed__dctor_Hook);

    // called when the player respawns after being busted or wasted
    patch::RedirectCall(0x443082, CReferences__RemoveReferencesToPlayer_Hook);

    patch::RedirectCall(0x74278B, CWeapon__TakePhotograph_Hook);

    patch::RedirectCall(0x5707AE, CTaskSimpleJetPack__DropJetPack_Hook);
    patch::RedirectCall(0x67E840, CTaskSimpleJetPack__DropJetPack_Hook);

    patch::RedirectCall(0x4B5B27, CPedDamageResponseCalculator__ComputeWillKillPed_Hook);

    patch::RedirectCall(0x4577E6, CPlayerPed__CanPlayerStartMission_Hook);
    patch::RedirectCall(0x4895B0, CPlayerPed__CanPlayerStartMission_Hook);

    patch::RedirectCall(0x60F2E0, CPlayerPed__ProcessWeaponSwitch_Hook); // disable switching weapon for network players

    patch::RedirectCall(0x6887E2, CTaskSimplePlayerOnFoot__PlayIdleAnimations_Hook);

    patch::RedirectCall(0x688700, CPad__JumpJustDown_Hook);
    patch::RedirectCall(0x6887D8, CTaskComplexJump_CTaskManager__SetTask_Hook);

    patch::ReplaceFunction(0x485A50, CRunningScript__DoDeatharrestCheck_Hook);
}
