#include "stdafx.h"
#include <game_sa/CTagManager.h>

int m_anStoredIntStats[224];
float m_afStoredFloatStats[83];

constexpr int MAX_INT_STATS = sizeof(m_anStoredIntStats) / sizeof(int);
constexpr int MAX_FLOAT_STATS = sizeof(m_afStoredFloatStats) / sizeof(float);

std::array<eStats, CStatsSync::SYNCED_STATS_COUNT> CStatsSync::m_aeSyncedStats =
{
    STAT_PISTOL_SKILL,
    STAT_SILENCED_PISTOL_SKILL,
    STAT_DESERT_EAGLE_SKILL,
    STAT_SHOTGUN_SKILL,
    STAT_SAWN_OFF_SHOTGUN_SKILL,
    STAT_COMBAT_SHOTGUN_SKILL,
    STAT_MACHINE_PISTOL_SKILL,
    STAT_SMG_SKILL,
    STAT_AK_47_SKILL,
    STAT_M4_SKILL,
    STAT_RIFLE_SKILL,
    STAT_STAMINA,
    STAT_MAX_HEALTH,
    STAT_LUNG_CAPACITY,
    STAT_SNAPSHOTS_TAKEN,
    STAT_HORSESHOES_COLLECTED,
    STAT_OYSTERS_COLLECTED,
    STAT_TAGS_SPRAYED,
};

namespace
{
    std::array<float, CStatsSync::SYNCED_STATS_COUNT> s_lastSyncedStats{};
    unsigned int s_lastPlayerInfoCheck = 0;
    bool s_forceNextSend = true;

    bool IsMonotonicStat(eStats stat)
    {
        return stat == STAT_SNAPSHOTS_TAKEN || stat == STAT_HORSESHOES_COLLECTED || stat == STAT_OYSTERS_COLLECTED
            || stat == STAT_TAGS_SPRAYED;
    }
}

void CStatsSync::ApplyNetworkPlayerContext(CNetworkPlayer* player)
{
    for (int i = 0; i < MAX_INT_STATS; ++i)
        m_anStoredIntStats[i] = CStats::StatTypesInt[i];

    for (int i = 0; i < MAX_FLOAT_STATS; ++i)
        m_afStoredFloatStats[i] = CStats::StatTypesFloat[i];

    for (int i = 0; i < MAX_INT_STATS; ++i)
        CStats::StatTypesInt[i] = player->m_stats.m_aStatsInt[i];

    for (int i = 0; i < MAX_FLOAT_STATS; ++i)
        CStats::StatTypesFloat[i] = player->m_stats.m_aStatsFloat[i];
}

void CStatsSync::ApplyLocalContext()
{
    for (int i = 0; i < MAX_INT_STATS; ++i)
        CStats::StatTypesInt[i] = m_anStoredIntStats[i];

    for (int i = 0; i < MAX_FLOAT_STATS; ++i)
        CStats::StatTypesFloat[i] = m_afStoredFloatStats[i];
}

void CStatsSync::FillPacket(Packets::Players::PlayerStats& packet)
{
    for (uint8_t i = 0; i < CStatsSync::SYNCED_STATS_COUNT; i++)
    {
        packet.stats[i] = CStats::GetStatValue(m_aeSyncedStats[i]);
    }
}

void CStatsSync::NotifyChanged()
{
    Packets::Players::PlayerStats packet{};
    FillPacket(packet);
    GetPacketFactory().Send(packet);
    s_forceNextSend = false;
}

void CStatsSync::Process()
{
    if (!CNetwork::m_bAuthenticated)
    {
        return;
    }

    unsigned int tickCount = GetTickCount();
    if (tickCount < s_lastPlayerInfoCheck + 200)
    {
        return;
    }
    s_lastPlayerInfoCheck = tickCount;

    bool changed = false;

    for (uint8_t i = 0; i < SYNCED_STATS_COUNT; i++)
    {
        const float statValue = CStats::GetStatValue(m_aeSyncedStats[i]);
        if (fabsf(statValue - s_lastSyncedStats[i]) >= 0.01f)
        {
            s_lastSyncedStats[i] = statValue;
            changed = true;
        }
    }

    if (changed || s_forceNextSend)
    {
        NotifyChanged();
    }
}

void CStatsSync::HandlePacket(const Packets::Players::PlayerStats& packet)
{
    for (uint8_t i = 0; i < SYNCED_STATS_COUNT; ++i)
    {
        const eStats statId = m_aeSyncedStats[i];
        const float newValue = packet.stats[i];
        const float currentValue = CStats::GetStatValue(statId);
        const float appliedValue = IsMonotonicStat(statId) ? std::max(currentValue, newValue) : newValue;

        if (fabsf(currentValue - appliedValue) >= 0.01f)
        {
            CStats::SetStatValue(statId, appliedValue);
            s_lastSyncedStats[i] = appliedValue;

            if (statId == STAT_TAGS_SPRAYED)
            {
                CTagManager::ms_numTagged = static_cast<int32_t>(appliedValue);
            }
        }
    }
}

int CStatsSync::GetSyncIdByInternal(eStats stat)
{
    for (int i = 0; i < CStatsSync::SYNCED_STATS_COUNT; i++)
    {
        if (m_aeSyncedStats[i] == stat)
            return i;
    }

    return -1;
}

bool CStatsSync::ShouldSyncStat(eStats stat)
{
    return GetSyncIdByInternal(stat) != -1;
}
