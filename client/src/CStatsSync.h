#pragma once

namespace Packets::Players
{
class PlayerStats;
}

class CStatsSync
{
public:
	static constexpr inline int SYNCED_STATS_COUNT = static_cast<int>(Packets::Players::SYNCED_STATS_COUNT);

	static std::array<eStats, SYNCED_STATS_COUNT> m_aeSyncedStats;
	static void ApplyNetworkPlayerContext(CNetworkPlayer* player);
	static void ApplyLocalContext();
	static void NotifyChanged();
	static void Process();
	static void HandlePacket(const Packets::Players::PlayerStats& packet);
	static int GetSyncIdByInternal(eStats stat);
	static bool ShouldSyncStat(eStats stat);
	static void FillPacket(Packets::Players::PlayerStats& packet);
};
