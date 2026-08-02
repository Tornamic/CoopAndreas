#include "stdafx.h"
#include "CRTTBroadcastManager.h"

void CRTTBroadcastManager::Update()
{
    static server_time_t nLastUpdate = 0;
    constexpr server_time_t RTT_UPDATE_INTERVAL = 1000;

    if (g_serverTime - nLastUpdate < RTT_UPDATE_INTERVAL || CNetworkPlayerManager::m_pPlayers.empty())
    {
        return;
    }

    nLastUpdate = g_serverTime;

    Packets::System::RTTBroadcast rttBroadcast{};
    for (CNetworkPlayer* pNetworkPlayer : CNetworkPlayerManager::m_pPlayers)
    {
        Packets::System::RTTBroadcast::SPlayerRTT& playerRTT = rttBroadcast.rtts[rttBroadcast.playerCount++];
        playerRTT.playerid = pNetworkPlayer->m_iPlayerId;
        playerRTT.rtt = static_cast<uint16_t>(
            std::min<enet_uint32>(pNetworkPlayer->m_pPeer->roundTripTime, UINT16_MAX));
    }

    GetPacketFactory().SendToAll(rttBroadcast);
}
