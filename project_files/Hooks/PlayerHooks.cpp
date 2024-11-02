#include "../stdafx.h"
#include "PlayerHooks.h"
#include "../CKeySync.h"

static void __fastcall CPlayerPed__ProcessControl_Hook(CPlayerPed* This)
{
    CPlayerPed* localPlayer = FindPlayerPed(0);

    if (This == localPlayer)
    {
        patch::SetRaw(0x6884C4, (void*)"\xD9\x96\x5C\x05\x00\x00", 6);
        plugin::CallMethod<0x60EA90, CPlayerPed*>(This);
        patch::Nop(0x6884C4, 6);
        return;
    }

    CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(This);

    if (player == nullptr)
        return;

    CWorld::PlayerInFocus = player->GetInternalId();

    CKeySync::ApplyNetworkPlayerContext(player);

    player->m_pPed->m_fHealth = player->m_lOnFoot->health;
    player->m_pPed->m_fArmour = player->m_lOnFoot->armour;

    player->m_pPed->m_vecMoveSpeed = player->m_lOnFoot->velocity;

    player->m_pPed->m_fAimingRotation =
        player->m_pPed->m_fCurrentRotation = player->m_lOnFoot->rotation;

    plugin::CallMethod<0x60EA90, CPlayerPed*>(This);

    CWorld::PlayerInFocus = 0;

    CKeySync::ApplyLocalContext();
}

static void __fastcall CWeapon__DoBulletImpact_Hook(CWeapon* weapon, int padding, CEntity* owner, CEntity* victim, CVector* startPoint, CVector* endPoint, CColPoint* colPoint, int incrementalHit)
{
    if (owner == FindPlayerPed(0))
    {
        CPackets::PlayerBulletShot* packet = new CPackets::PlayerBulletShot;

        packet->targetid = -1;

        if (victim != nullptr)
        {
            switch (victim->m_nType)
            {
            case eEntityType::ENTITY_TYPE_PED: // ped or player
            {
                if (auto playerTarget = CNetworkPlayerManager::GetPlayer(victim))
                    packet->targetid = playerTarget->m_nPlayerId;
                break;
            }
            case eEntityType::ENTITY_TYPE_VEHICLE:
            {
                if (auto vehicleTarget = CNetworkVehicleManager::GetVehicle(victim))
                    packet->targetid = vehicleTarget->m_nVehicleId;
                break;
            }
            }
            packet->entityType = victim->m_nType;
        }

        packet->startPos = *startPoint;
        packet->endPos = *endPoint;
        packet->colPoint = *colPoint;
        packet->incrementalHit = incrementalHit;

        CNetwork::SendPacket(ePacketType::PLAYER_BULLET_SHOT, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED);

        weapon->DoBulletImpact(owner, victim, startPoint, endPoint, colPoint, incrementalHit);

        return;
    }
}

static void __fastcall CPedIK__PointGunInDirection_Hook(CPedIK* This, int padding, float dirX, float dirY, char flag, float float1)
{
    if (This->m_pPed == FindPlayerPed(0))
    {
        // save last aim data for syncing
        CLocalPlayer::m_fLastAimX = dirX;
        CLocalPlayer::m_fLastAimY = dirY;

        This->PointGunInDirection(dirX, dirY, flag, float1);
        return;
    }

    CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(This->m_pPed);

    if (player == nullptr)
    {
        This->PointGunInDirection(dirX, dirY, flag, float1);
        return;
    }

    if (player->m_lOnFoot == nullptr)
        return;

    player->m_pPed->m_fAimingRotation =
        player->m_pPed->m_fCurrentRotation = player->m_lOnFoot->rotation;

    This->PointGunInDirection(player->m_lOnFoot->aimX, player->m_lOnFoot->aimY, flag, float1);
}

static void __fastcall CPlayerPed__dctor_Hook(CPlayerPed* This, int)
{
    This->m_pPlayerData->m_nMeleeWeaponAnimReferencedExtra = 0;
    plugin::CallMethod<0x6093B0, CPlayerPed*>(This);
}

void PlayerHooks::InjectHooks()
{
    patch::SetPointer(0x86D190, CPlayerPed__ProcessControl_Hook);

    patch::RedirectCall(0x73CD92, CWeapon__DoBulletImpact_Hook);
    patch::RedirectCall(0x741199, CWeapon__DoBulletImpact_Hook);
    patch::RedirectCall(0x7411DF, CWeapon__DoBulletImpact_Hook);
    patch::RedirectCall(0x7412DF, CWeapon__DoBulletImpact_Hook);
    patch::RedirectCall(0x741E30, CWeapon__DoBulletImpact_Hook);

    patch::RedirectCall(0x5FDF7A, CPedIK__PointGunInDirection_Hook);
    patch::RedirectCall(0x61F351, CPedIK__PointGunInDirection_Hook);
    patch::RedirectCall(0x62876D, CPedIK__PointGunInDirection_Hook);

    // fix CPlayerPed dctor crash
    patch::RedirectCall(0x60A9A3, CPlayerPed__dctor_Hook);
}
