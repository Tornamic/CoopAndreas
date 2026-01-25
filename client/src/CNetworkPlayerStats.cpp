#include "stdafx.h"
#include "CNetworkPlayerStats.h"

CNetworkPlayerStats::CNetworkPlayerStats()
{
    m_aStatsFloat.fill(0);
    m_aStatsInt.fill(0);

    m_aStatsFloat[STAT_MAX_HEALTH] = 569.0f;
    m_aStatsFloat[STAT_STAMINA] = 100.0f;
}

float CNetworkPlayerStats::GetFloatStat(size_t index) const
{
    assert(index < m_aStatsFloat.size());
    return m_aStatsFloat[index];
}

void CNetworkPlayerStats::SetFloatStat(size_t index, float value)
{
    assert(index < m_aStatsFloat.size());
    m_aStatsFloat[index] = value;
}

int CNetworkPlayerStats::GetIntStat(size_t index) const
{
    assert(index < m_aStatsInt.size());
    return m_aStatsInt[index];
}

void CNetworkPlayerStats::SetIntStat(size_t index, int value)
{
    assert(index < m_aStatsInt.size());
    m_aStatsInt[index] = value;
}