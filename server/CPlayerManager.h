#pragma once
class CPlayerManager
{
public:
	static std::vector<CPlayer*> m_pPlayers;
	static void Add(CPlayer* player);
	static void Remove(CPlayer* player);
	static CPlayer* GetPlayer(int playerid);
	static CPlayer* GetPlayer(ENetPeer* peer);
	static int GetFreeID();
};

