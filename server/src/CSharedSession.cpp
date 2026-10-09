#include "stdafx.h"
#include "CSharedSession.h"
#include "CNetworkPlayer.h"
#include "CPacketFactory.h"
#include "network/packets/players.h"
#include <cmath>
#include <cstring>

namespace
{
    float s_afStats[Packets::Players::SYNCED_STATS_COUNT]{};
    bool s_bActive = false;
    int s_nSourcePlayerId = 0;
}

void CSharedSession::ApplyFromPlayer(CNetworkPlayer* player, Packets::Players::PlayerStats* packet)
{
    if (player == nullptr || packet == nullptr)
    {
        return;
    }

    if (!player->m_bSessionBaselineSet)
    {
        player->m_bSessionBaselineSet = true;
        std::memcpy(player->m_afLastReportedStats, packet->stats, sizeof(player->m_afLastReportedStats));

        if (!s_bActive)
        {
            s_bActive = true;
            s_nSourcePlayerId = player->m_iPlayerId;
            std::memcpy(s_afStats, packet->stats, sizeof(s_afStats));
        }
    }
    else
    {
        for (size_t i = 0; i < ARRAY_SIZE(s_afStats); ++i)
        {
            if (std::fabs(packet->stats[i] - player->m_afLastReportedStats[i]) >= 0.01f)
            {
                s_afStats[i] = std::max(s_afStats[i], packet->stats[i]);
                player->m_afLastReportedStats[i] = packet->stats[i];
            }
        }
    }

    std::memcpy(packet->stats, s_afStats, sizeof(packet->stats));
}

void CSharedSession::SendToPlayer(CNetworkPlayer* target)
{
    if (!s_bActive || target == nullptr)
    {
        return;
    }

    Packets::Players::PlayerStats packet{};
    packet.playerid = s_nSourcePlayerId;
    std::memcpy(packet.stats, s_afStats, sizeof(packet.stats));
    GetPacketFactory().Send(packet, target);
}

bool CSharedSession::IsActive()
{
    return s_bActive;
}
