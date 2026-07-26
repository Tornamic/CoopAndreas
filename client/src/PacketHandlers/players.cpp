#include "network/packets/players.h"
#include "network/packet_types.h"
#include "stdafx.h"
#include <CProjectileInfo.h>
#include <CAimSync.h>

PACKET_HANDLER(ePacketType::PLAYER_ONFOOT_UPDATE, Packets::Players::OnFootUpdate* pOnFootUpdate)
{
    CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(pOnFootUpdate->playerid);
    if (pNetworkPlayer == nullptr)
    {
        return;
    }

    CPlayerPed* pPlayerPed = pNetworkPlayer->m_pPed;

    if (pPlayerPed == nullptr)
    {
        pNetworkPlayer->CreatePed(pNetworkPlayer->m_iPlayerId, pOnFootUpdate->vecPos);
        pPlayerPed = pNetworkPlayer->m_pPed;
    }

    if (pPlayerPed->m_pVehicle != nullptr && pPlayerPed->m_nPedFlags.bInVehicle)
    {
        pNetworkPlayer->RemoveFromVehicle(pPlayerPed->m_pVehicle);
    }

    pNetworkPlayer->m_pPed->SetPosn(pOnFootUpdate->vecPos);
    pNetworkPlayer->m_pPed->m_vecMoveSpeed = pOnFootUpdate->vecMoveSpeed;

    pNetworkPlayer->ApplyWeaponSnapshot(pOnFootUpdate->weaponSnapshot);

    pNetworkPlayer->m_pPed->m_fCurrentRotation = pOnFootUpdate->currentRotation.m_angle;
    pNetworkPlayer->m_pPed->m_fAimingRotation = pOnFootUpdate->aimingRotation.m_angle;

    CUtil::SetPlayerJetpack(pNetworkPlayer, pOnFootUpdate->bHasJetpack);

    if (CUtil::IsDucked(pNetworkPlayer->m_pPed) != pOnFootUpdate->bDucking)
    {
        if (pOnFootUpdate->bDucking)
        {
            pNetworkPlayer->m_pPed->m_pIntelligence->SetTaskDuckSecondary(0);
            // abort fighting and jumping tasks maybe?
        }
        else
        {
            pNetworkPlayer->m_pPed->m_pIntelligence->ClearTaskDuckSecondary();
        }
    }

    pNetworkPlayer->m_pPed->m_nFightingStyle = pOnFootUpdate->iFightingStyle;
    pNetworkPlayer->m_pPed->m_nAllowedAttackMoves |= 15u;

    pNetworkPlayer->m_oldControllerState = pOnFootUpdate->keySnapshot.oldControllerState;
    pNetworkPlayer->m_newControllerState = pOnFootUpdate->keySnapshot.newControllerState;

    pNetworkPlayer->m_onFootSnapshotInterpolated = *pOnFootUpdate;
}

PACKET_HANDLER(ePacketType::PLAYER_KEY_SYNC, Packets::Players::KeyPressed* pKeyPressed)
{
    CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(pKeyPressed->playerid);
    if (pNetworkPlayer == nullptr)
    {
        return;
    }

    pNetworkPlayer->m_oldControllerState = pKeyPressed->keySnapshot.oldControllerState;
    pNetworkPlayer->m_newControllerState = pKeyPressed->keySnapshot.newControllerState;

    pNetworkPlayer->m_pPed->m_fCurrentRotation = pKeyPressed->currentRotation.m_angle;
    pNetworkPlayer->m_pPed->m_fAimingRotation = pKeyPressed->aimingRotation.m_angle;

    pNetworkPlayer->m_onFootSnapshotInterpolated.currentRotation = pKeyPressed->currentRotation;
    pNetworkPlayer->m_onFootSnapshotInterpolated.aimingRotation = pKeyPressed->aimingRotation;
}

PACKET_HANDLER(ePacketType::PLAYER_CAMERA_SYNC, Packets::Players::PlayerCameraSync* pPlayerCameraSync)
{
    CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(pPlayerCameraSync->playerid);
    if (pNetworkPlayer == nullptr)
    {
        return;
    }

    pNetworkPlayer->m_startedInterpolatingCameraAt = GetTickCount();
    pNetworkPlayer->m_cameraSnapshotOld = pNetworkPlayer->m_cameraSnapshot;
    if (pPlayerCameraSync->bFullUpdate)
    {
        pNetworkPlayer->m_cameraSnapshot = *pPlayerCameraSync;
    }
    else
    {
        pNetworkPlayer->m_cameraSnapshot.cameraMode = pPlayerCameraSync->cameraMode;
        pNetworkPlayer->m_cameraSnapshot.orientation = pPlayerCameraSync->orientation;
        pNetworkPlayer->m_cameraSnapshot.front.x = pPlayerCameraSync->front.x;
        pNetworkPlayer->m_cameraSnapshot.front.y = pPlayerCameraSync->front.y;
    }
}

PACKET_HANDLER(ePacketType::SET_PLAYER_TASK, Packets::Players::SetPlayerTask* pSetPlayerTask)
{
    CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(pSetPlayerTask->playerid);
    if (pNetworkPlayer == nullptr)
    {
        return;
    }

    pNetworkPlayer->HandleTask(*pSetPlayerTask);
}

PACKET_HANDLER(ePacketType::PLAYER_PLACE_WAYPOINT, Packets::Players::PlayerPlaceWaypoint* pPlayerPlaceWaypoint)
{
    CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(pPlayerPlaceWaypoint->playerid);
    if (pNetworkPlayer == nullptr)
    {
        return;
    }

    pNetworkPlayer->m_waypointState = *pPlayerPlaceWaypoint;

#ifdef PACKET_DEBUG_MESSAGES
    CChat::AddMessage("WAYPOINT PLACE %d %.0f %.0f\n", packet->place, packet->position.x, packet->position.y);
#endif
}

PACKET_HANDLER(ePacketType::RESPAWN_PLAYER, Packets::Players::RespawnPlayer* pRespawnPlayer)
{
    CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(pRespawnPlayer->playerid);
    if (pNetworkPlayer == nullptr)
    {
        return;
    }

    pNetworkPlayer->Respawn();
}

PACKET_HANDLER(ePacketType::PLAYER_BULLET_SHOT, Packets::Players::PlayerBulletShot* pPlayerBulletShot)
{
    CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(pPlayerBulletShot->playerid);
    if (pNetworkPlayer == nullptr)
    {
        return;
    }

    pNetworkPlayer->m_pPed->SetCurrentWeapon(pPlayerBulletShot->iWeaponType);

    CEntity* owner = pNetworkPlayer->m_pPed;
    CEntity* victim = nullptr;
    CVector startPoint = pPlayerBulletShot->startPos;
    CVector endPoint = pPlayerBulletShot->endPos;
    CColPoint colPoint;
    int incrementalHit = pPlayerBulletShot->incrementalHit;

    if (pPlayerBulletShot->bHitSomething)
    {
        CVector endPosCalced = (endPoint - startPoint);
        endPosCalced.Normalise();
        endPosCalced = endPoint + endPosCalced * 10.0f;
        endPoint = endPosCalced;
        CWorld::ProcessLineOfSight(startPoint, endPoint, colPoint, victim, 1, 1, 1, 1, 1, 0, 0, 1);
        colPoint.m_vecPoint = pPlayerBulletShot->endPos;

        if (pPlayerBulletShot->bHitNetworkEntity)
        {
            victim = pPlayerBulletShot->hitEntity.GetEntity();
            if (pPlayerBulletShot->hitEntity.entityType == NETWORK_ENTITY_TYPE_PLAYER ||
                pPlayerBulletShot->hitEntity.entityType == NETWORK_ENTITY_TYPE_PED)
            {
                colPoint.m_nSurfaceTypeB = pPlayerBulletShot->pedPiece;
            }
        }
    }
    else
    {
        colPoint.m_vecPoint.Set(0.0f, 0.0f, 0.0f);
        colPoint.m_vecNormal.Set(0.0f, 0.0f, 0.0f);
        colPoint.m_nLightingB.day = 0;
        colPoint.m_nLightingB.night = 0;
        colPoint.m_nPieceTypeB = 0;
        colPoint.m_nSurfaceTypeB = 0;
    }

    pNetworkPlayer->m_pPed->GetWeapon().DoBulletImpact(
        owner, victim, &startPoint, &pPlayerBulletShot->endPos, &colPoint, incrementalHit);
}

PACKET_HANDLER(ePacketType::ADD_PROJECTILE, Packets::Players::AddProjectile* pAddProjectile)
{
    if (pAddProjectile->creator.GetEntity() == nullptr)
    {
        return;
    }

    CVector origin = pAddProjectile->origin;
    CVector dir = pAddProjectile->dir;

    if (pAddProjectile->creator.entityType == NETWORK_ENTITY_TYPE_PLAYER)
    {
        if (auto networkPlayer = CNetworkPlayerManager::GetPlayer(pAddProjectile->creator.GetEntity()))
        {
            CAimSync::ApplyNetworkPlayerContext(networkPlayer);
        }
    }

    CEntity* pTarget = nullptr;
    if (pAddProjectile->bTarget)
    {
        pTarget = pAddProjectile->target.GetEntity();
    }
    if (pAddProjectile->bDir)
    {
        CProjectileInfo::AddProjectile(pAddProjectile->creator.GetEntity(), pAddProjectile->projectileType, origin, pAddProjectile->force, nullptr, pTarget);
    }
    else
    {
        CProjectileInfo::AddProjectile(pAddProjectile->creator.GetEntity(), pAddProjectile->projectileType, origin, pAddProjectile->force, &pAddProjectile->dir, pTarget);
    }

    if (pAddProjectile->creator.entityType == NETWORK_ENTITY_TYPE_PLAYER && CNetworkPlayerManager::GetPlayer(pAddProjectile->creator.GetEntity()))
    {
        CAimSync::ApplyLocalContext();
    }
}

PACKET_HANDLER(ePacketType::PLAYER_STATS, Packets::Players::PlayerStats* pPlayerStats)
{
    if (auto pNetworkPlayer = CNetworkPlayerManager::GetPlayer(pPlayerStats->playerid))
    {
        for (size_t i = 0; i < CStatsSync::SYNCED_STATS_COUNT; i++)
        {
            eStats statId = CStatsSync::m_aeSyncedStats[i];
            pNetworkPlayer->m_stats[statId] = pPlayerStats->stats[i];
        }
    }
}

PACKET_HANDLER(ePacketType::REBUILD_PLAYER, Packets::Players::RebuildPlayer* pRebuildPlayer)
{
    if (auto pNetworkPlayer = CNetworkPlayerManager::GetPlayer(pRebuildPlayer->playerid))
    {
        pNetworkPlayer->m_stats[STAT_FAT] = pRebuildPlayer->clothesDesc.m_fFatStat;
        pNetworkPlayer->m_stats[STAT_MUSCLE] = pRebuildPlayer->clothesDesc.m_fMuscleStat;

        //CStatsSync::ApplyNetworkPlayerContext(networkPlayer);

        pNetworkPlayer->m_pPedClothesDesc = pRebuildPlayer->clothesDesc;

        if (auto pPlayerPed = pNetworkPlayer->m_pPed)
        {
            *pPlayerPed->m_pPlayerData->m_pPedClothesDesc = pRebuildPlayer->clothesDesc;
            if (pPlayerPed->m_pRwClump)
            {
                CClothes::RebuildPlayer(pPlayerPed, false);
            }
        }

        //CStatsSync::ApplyLocalContext();
    }
}