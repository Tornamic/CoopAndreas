#include "network/packets/blips.h"
#include "network/packet_types.h"
#include "stdafx.h"

PACKET_HANDLER(ePacketType::UPDATE_ENTITY_BLIP, Packets::Blips::UpdateEntityBlip* pUpdateEntityBlip, CNetworkPlayer* pNetworkPlayer)
{
    if (pNetworkPlayer->m_bIsHost)
    {
        if (auto pTargetPlayer = CNetworkPlayerManager::GetPlayer(pUpdateEntityBlip->forWhoPlayerId))
        {
            GetPacketFactory().Send(*pUpdateEntityBlip, pTargetPlayer);
        }
    }
}

PACKET_HANDLER(ePacketType::REMOVE_ENTITY_BLIP, Packets::Blips::RemoveEntityBlip* pRemoveEntityBlip, CNetworkPlayer* pNetworkPlayer)
{
    if (pNetworkPlayer->m_bIsHost)
    {
        if (auto pTargetPlayer = CNetworkPlayerManager::GetPlayer(pRemoveEntityBlip->forWhoPlayerId))
        {
            GetPacketFactory().Send(*pRemoveEntityBlip, pTargetPlayer);
        }
    }
}

PACKET_HANDLER(ePacketType::CLEAR_ENTITY_BLIPS, Packets::Blips::ClearEntityBlips* pClearEntityBlips, CNetworkPlayer* pNetworkPlayer)
{
    if (pNetworkPlayer->m_bIsHost)
    {
        if (auto pTargetPlayer = CNetworkPlayerManager::GetPlayer(pClearEntityBlips->forWhoPlayerId))
        {
            GetPacketFactory().Send(*pClearEntityBlips, pTargetPlayer);
        }
    }
}

PACKET_HANDLER(ePacketType::UPDATE_CHECKPOINT, Packets::Blips::UpdateCheckpoint* pUpdateCheckpoint, CNetworkPlayer* pNetworkPlayer)
{
    if (pNetworkPlayer->m_bIsHost)
    {
        if (auto pTargetPlayer = CNetworkPlayerManager::GetPlayer(pUpdateCheckpoint->forWhoPlayerId))
        {
            GetPacketFactory().Send(*pUpdateCheckpoint, pTargetPlayer);
        }
    }
}

PACKET_HANDLER(ePacketType::REMOVE_CHECKPOINT, Packets::Blips::RemoveCheckpoint* pRemoveCheckpoint, CNetworkPlayer* pNetworkPlayer)
{
    if (pNetworkPlayer->m_bIsHost)
    {
        if (auto pTargetPlayer = CNetworkPlayerManager::GetPlayer(pRemoveCheckpoint->forWhoPlayerId))
        {
            GetPacketFactory().Send(*pRemoveCheckpoint, pTargetPlayer);
        }
    }
}

PACKET_HANDLER(ePacketType::CREATE_STATIC_BLIP, Packets::Blips::StaticBlipsSnapshot* pCreateStaticBlip, CNetworkPlayer* pNetworkPlayer)
{
    if (pNetworkPlayer->m_bIsHost)
    {
        GetPacketFactory().SendToAll(*pCreateStaticBlip, pNetworkPlayer);
    }
}
