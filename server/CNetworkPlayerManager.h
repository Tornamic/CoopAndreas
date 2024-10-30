#pragma once
class CNetworkPlayerManager
{
public:
	static std::vector<CNetworkPlayer*> m_pPlayers;
	static void Add(CNetworkPlayer* player);
	static void Remove(CNetworkPlayer* player);
	static CNetworkPlayer* GetPlayer(int playerid);
	static CNetworkPlayer* GetPlayer(ENetPeer* peer);
	static int GetFreeID();
	static CNetworkPlayer* GetHost();
	static void AssignHostToFirstPlayer();
};

