#include "network/packets/blips.h"
#include "network/packet_types.h"
#include "stdafx.h"

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
