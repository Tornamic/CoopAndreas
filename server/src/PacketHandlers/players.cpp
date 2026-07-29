#include "network/packets/players.h"
#include "network/packet_types.h"
#include "stdafx.h"
#include "network/packet_handler.h"

PACKET_HANDLER(
    ePacketType::PLAYER_ONFOOT_UPDATE, Packets::Players::OnFootUpdate* pOnFootUpdate, CNetworkPlayer* pNetworkPlayer)
{
    pOnFootUpdate->playerid.value = pNetworkPlayer->m_iPlayerId;
    GetPacketFactory().SendToAll(*pOnFootUpdate, pNetworkPlayer);
}

PACKET_HANDLER(ePacketType::PLAYER_KEY_SYNC, Packets::Players::KeyPressed* pKeyPressed, CNetworkPlayer* pNetworkPlayer)
{
    pKeyPressed->playerid = pNetworkPlayer->m_iPlayerId;
    GetPacketFactory().SendToAll(*pKeyPressed, pNetworkPlayer);
}

PACKET_HANDLER(ePacketType::PLAYER_CAMERA_SYNC, Packets::Players::PlayerCameraSync* pPlayerCameraSync, CNetworkPlayer* pNetworkPlayer)
{
    pPlayerCameraSync->playerid = pNetworkPlayer->m_iPlayerId;
    GetPacketFactory().SendToAll(*pPlayerCameraSync, pNetworkPlayer);
}

PACKET_HANDLER(ePacketType::SET_PLAYER_TASK, Packets::Players::SetPlayerTask* pSetPlayerTask, CNetworkPlayer* pNetworkPlayer)
{
    pSetPlayerTask->playerid = pNetworkPlayer->m_iPlayerId;
    GetPacketFactory().SendToAll(*pSetPlayerTask, pNetworkPlayer);
}

PACKET_HANDLER(ePacketType::ENEX_TRANSITION, Packets::Players::EnExTransition* pEnExTransition,
    CNetworkPlayer* pNetworkPlayer)
{
    pEnExTransition->playerid = pNetworkPlayer->m_iPlayerId;
    GetPacketFactory().SendToAll(*pEnExTransition, pNetworkPlayer);
}

PACKET_HANDLER(ePacketType::PLAYER_PLACE_WAYPOINT, Packets::Players::PlayerPlaceWaypoint* pPlayerPlaceWaypoint, CNetworkPlayer* pNetworkPlayer)
{
    pNetworkPlayer->m_waypointState = *pPlayerPlaceWaypoint;

    pPlayerPlaceWaypoint->playerid = pNetworkPlayer->m_iPlayerId;
    GetPacketFactory().SendToAll(*pPlayerPlaceWaypoint, pNetworkPlayer);
}

PACKET_HANDLER(ePacketType::RESPAWN_PLAYER, Packets::Players::RespawnPlayer* pRespawnPlayer, CNetworkPlayer* pNetworkPlayer)
{
    pRespawnPlayer->playerid = pNetworkPlayer->m_iPlayerId;
    GetPacketFactory().SendToAll(*pRespawnPlayer, pNetworkPlayer);
}

PACKET_HANDLER(ePacketType::PLAYER_BULLET_SHOT, Packets::Players::PlayerBulletShot* pPlayerBulletShot, CNetworkPlayer* pNetworkPlayer)
{
    pPlayerBulletShot->playerid = pNetworkPlayer->m_iPlayerId;
    GetPacketFactory().SendToAll(*pPlayerBulletShot, pNetworkPlayer);
}

PACKET_HANDLER(ePacketType::ADD_PROJECTILE, Packets::Players::AddProjectile* pAddProjectile, CNetworkPlayer* pNetworkPlayer)
{
    GetPacketFactory().SendToAll(*pAddProjectile, pNetworkPlayer);
}

PACKET_HANDLER(ePacketType::PLAYER_STATS, Packets::Players::PlayerStats* pPlayerStats, CNetworkPlayer* pNetworkPlayer)
{
    for (size_t i = 0; i < ARRAY_SIZE(pNetworkPlayer->m_afStats); i++)
    {
        pNetworkPlayer->m_afStats[i] = pPlayerStats->stats[i];
    }

    pNetworkPlayer->m_ucSyncFlags.bStatsModified = true;
    pPlayerStats->playerid = pNetworkPlayer->m_iPlayerId;
    GetPacketFactory().SendToAll(*pPlayerStats, pNetworkPlayer);
}

PACKET_HANDLER(ePacketType::REBUILD_PLAYER, Packets::Players::RebuildPlayer* pRebuildPlayer, CNetworkPlayer* pNetworkPlayer)
{
    pNetworkPlayer->m_pPedClothesDesc = pRebuildPlayer->clothesDesc;
    pNetworkPlayer->m_ucSyncFlags.bClothesModified = true;
    pRebuildPlayer->playerid = pNetworkPlayer->m_iPlayerId;
    GetPacketFactory().SendToAll(*pRebuildPlayer, pNetworkPlayer);
}

