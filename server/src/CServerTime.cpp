#include "stdafx.h"

server_time_t g_serverTime = 0;

uint32_t CServerTime::m_serverStartedAt = 0;

void CServerTime::Init()
{
    m_serverStartedAt = GetTickCount();
}

void CServerTime::Update()
{
    g_serverTime = GetTickCount() - m_serverStartedAt;
}

PACKET_HANDLER(ePacketType::SERVER_TIME_REQUEST, Packets::System::ServerTimeRequest* pServerTimeRequest,
    CNetworkPlayer* pNetworkPlayer)
{
    pServerTimeRequest->serverTimeRespond = g_serverTime;
    GetPacketFactory().Send(*pServerTimeRequest, pNetworkPlayer);
}