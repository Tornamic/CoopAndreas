#pragma once

class CNetworkPlayer;

class CKeySync
{
public:
	static void ApplyNetworkPlayerContext(CNetworkPlayer* player);
	static void ApplyLocalContext();
	static void ProcessPlayer(CNetworkPlayer* player);
};

