#pragma once
class CNetworkPlayerManager
{
public:
	static std::vector<CNetworkPlayer*> m_pPlayers;
	static CPad m_pPads[MAX_SERVER_PLAYERS + 2];

	static void Add(CNetworkPlayer* player);
	static void Remove(CNetworkPlayer* player);
	static CNetworkPlayer* GetPlayer(CPlayerPed* playerPed);
	static CNetworkPlayer* GetPlayer(int playerid);
	static CNetworkPlayer* GetPlayer(CPed* ped);
};

