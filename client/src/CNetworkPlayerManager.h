#pragma once
class CNetworkPlayerManager
{
public:
    static std::vector<CNetworkPlayer*> m_pPlayers;
    static CPad m_pPads[Config::MAX_SERVER_PLAYERS + 2];
    static playerid_t m_localPlayerId;

    static void Add(CNetworkPlayer* player);
    static void Remove(CNetworkPlayer* player);
    static CNetworkPlayer* GetPlayer(playerid_t playerid);
    static CNetworkPlayer* GetPlayer(SenderPlayerId senderPlayerid);
    static CNetworkPlayer* GetPlayer(CEntity* entity);
    static int GetFreeSlot();
};