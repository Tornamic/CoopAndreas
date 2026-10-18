#pragma once

class CHorseshoeSync
{
public:
    static constexpr int COUNT = 50;
    static constexpr int FIRST_GLOBAL_VAR = 2882;  // $pickup_horseshoe in INITIAL.txt

    static void Process();  // call every frame
    static void SyncCurrentState();
    static void OnStateReceived(uint32_t low, uint32_t high);

private:
    static inline uint64_t ms_collected = 0;
    static inline uint64_t ms_pendingRemote = 0;
    static inline float ms_lastStat = -1.0f;
};