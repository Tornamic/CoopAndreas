#pragma once
class CNetworkPlayerManager
{
public:
    static std::vector<CNetworkPlayer*> m_pPlayers;
    static CPad m_pPads[Config::MAX_SERVER_PLAYERS + 2];
    static int m_nMyId;

    static void Add(CNetworkPlayer* player);
    static void Remove(CNetworkPlayer* player);
    static CNetworkPlayer* GetPlayer(int playerid);
    static CNetworkPlayer* GetPlayer(SenderPlayerId playerid);
    static CNetworkPlayer* GetPlayer(CEntity* entity);
};