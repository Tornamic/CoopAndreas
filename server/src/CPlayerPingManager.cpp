#include "stdafx.h"
#include "CPlayerPingManager.h"

void CPlayerPingManager::Update()
{
    static server_time_t nLastUpdate = 0;
    constexpr server_time_t PING_UPDATE_INTERVAL = 1000;

    if (g_serverTime - nLastUpdate < PING_UPDATE_INTERVAL || CNetworkPlayerManager::m_pPlayers.empty())
    {
        return;
    }

    nLastUpdate = g_serverTime;

    Packets::System::PlayerPing playerPing{};
    for (CNetworkPlayer* pNetworkPlayer : CNetworkPlayerManager::m_pPlayers)
    {
        Packets::System::PlayerPing::SPlayerPing& ping = playerPing.pings[playerPing.playerCount++];
        ping.playerid = pNetworkPlayer->m_iPlayerId;
        ping.ping = static_cast<uint16_t>(
            std::min<enet_uint32>(pNetworkPlayer->m_pPeer->roundTripTime, UINT16_MAX));
    }

    GetPacketFactory().SendToAll(playerPing);
}
