#include "stdafx.h"
#include "CServerTime.h"
#include <CWeatherSync.h>

server_time_t g_serverTime = 0;

uint32_t CServerTime::m_requestTick = 0;
uint32_t CServerTime::m_lastUpdated = 0;
int32_t CServerTime::m_serverTimeOffset = 0;

void CServerTime::Process()
{
    RecalculateServerTime();

    if (CNetwork::m_bAuthenticated)
    {
        uint32_t clientTick = GetTickCount();
        if (clientTick - m_lastUpdated > REQUEST_FREQUENCY)
        {
            SendRequest();
            m_lastUpdated = clientTick;
        }
    }
}

void CServerTime::RecalculateServerTime()
{
    g_serverTime = GetTickCount() + m_serverTimeOffset;

    CWeatherSync::ServerTimeRecalculated(g_serverTime);
}

void CServerTime::RecalculateOffset(int32_t newOffset)
{
    if (m_serverTimeOffset == 0)
    {
        m_serverTimeOffset = newOffset;
        return;
    }

    // a + t(b-a)
    m_serverTimeOffset += static_cast<int32_t>(0.1f * (newOffset - m_serverTimeOffset));
}

void CServerTime::SendRequest()
{
    m_requestTick = GetTickCount();

    Packets::System::ServerTimeRequest serverTimeRequest{};
    GetPacketFactory().Send(serverTimeRequest);
}

PACKET_HANDLER(ePacketType::SERVER_TIME_REQUEST, Packets::System::ServerTimeRequest* pServerTimeRequest)
{
    if (CServerTime::m_requestTick == 0)
    {
        logger::warn("m_requestTick wasn't set, skipping");
        return;
    }

    uint32_t clientTick = GetTickCount();
    uint32_t rtt = clientTick - CServerTime::m_requestTick;

    uint32_t serverTime = pServerTimeRequest->serverTimeRespond + rtt / 2;

    int32_t newOffset = static_cast<int32_t>(static_cast<int64_t>(serverTime) - static_cast<int64_t>(clientTick));

    CServerTime::RecalculateOffset(newOffset);
    CServerTime::RecalculateServerTime();

    // logger::info("tick %u, rtt %u, serverTime(r) %u, newOffset %d, offset %d, serverTime %d", clientTick, rtt,
    //     serverTime, newOffset, CServerTime::m_serverTimeOffset, g_serverTime);

    CServerTime::m_requestTick = 0;
}