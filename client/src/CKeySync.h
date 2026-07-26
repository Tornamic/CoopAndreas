#pragma once
class CKeySync
{
public:
	static void ApplyNetworkPlayerContext(CNetworkPlayer* player);
	static void ApplyLocalContext();
	static void CollectState(Packets::Players::SKeySnapshot& keySnapshot);
};

