#pragma once

class CServerTime
{
public:
    static void Init();
    static void Update();

private:
    static uint32_t m_serverStartedAt;
};

extern server_time_t g_serverTime;