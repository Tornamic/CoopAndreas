#pragma once

class CServerTime
{
public:
    static constexpr uint32_t REQUEST_FREQUENCY = 500;
    static void Process();
    
    static void RecalculateOffset(int32_t newOffset);
    static void RecalculateServerTime();
    static void SendRequest();

    static uint32_t m_requestTick;
    static uint32_t m_lastUpdated;
    static int32_t m_serverTimeOffset;
};

extern server_time_t g_serverTime;