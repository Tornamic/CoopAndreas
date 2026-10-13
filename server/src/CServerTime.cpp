#include "stdafx.h"
#include <chrono>

server_time_t g_serverTime = 0;

uint32_t CServerTime::m_serverStartedAt = 0;

// wraps like GetTickCount, unsigned subtraction keeps the elapsed time correct
static uint32_t GetTickCountMs()
{
    using namespace std::chrono;
    return static_cast<uint32_t>(duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
}

void CServerTime::Init()
{
    m_serverStartedAt = GetTickCountMs();
}

void CServerTime::Update()
{
    g_serverTime = GetTickCountMs() - m_serverStartedAt;
}

PACKET_HANDLER(ePacketType::SERVER_TIME_REQUEST, Packets::System::ServerTimeRequest* pServerTimeRequest,
    CNetworkPlayer* pNetworkPlayer)
{
    pServerTimeRequest->serverTimeRespond = g_serverTime;
    GetPacketFactory().Send(*pServerTimeRequest, pNetworkPlayer);
}