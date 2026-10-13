#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>

#include "CNetworkPlayer.h"

class CNetworkPlayerManager
{
public:
    static std::vector<CNetworkPlayer*> m_pPlayers;
    static void Add(CNetworkPlayer* player);
    static void Remove(CNetworkPlayer* player);
    static CNetworkPlayer* GetPlayer(playerid_t playerid);
    static CNetworkPlayer* GetPlayer(ENetPeer* peer);
    static playerid_t AllocatePlayerId();
    static CNetworkPlayer* GetHost();
    static void AssignHostToFirstPlayer();

private:
    static uint32_t ms_nNextPlayerId;
};