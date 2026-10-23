#pragma once

class COnscreenTimerSync
{
public:
    // host: created by a synced mission, client: received from the host
    inline static bool ms_bClockNetwork = false;
    inline static bool ms_abCounterNetwork[4]{};

    static void InjectHooks();

    static void Process();
    static void SendState();
    static void ClearNetworkEntries();
    static void HandlePacket(const Packets::Scripts::OnscreenTimerSync& packet);
};
