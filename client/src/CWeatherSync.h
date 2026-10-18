#pragma once

// Weather and clock come from the host. The other players keep running their clock between the packets, but must
// not pick the next weather themselves: at the top of the hour CWeather::Update (0x72B8B2) shifts New -> Old and takes
// the new one from the weather list of the camera's region, so each player would get its own sky until the next packet.
class CWeatherSync
{
public:
    static void Process();
    static void SyncCurrentState();
    static void HandlePacket(Packets::World::GameWeatherTime* pGameWeatherTime);
    static void ServerTimeRecalculated(server_time_t serverTime);
    static void OnBecameHost();
    static void Reset();

private:
    static bool ms_bHasHostState;
    static int16_t ms_nHostNewWeather;
    static int16_t ms_nHostForcedWeather;

    static uint32_t ms_nLastSyncTime;
    static int16_t ms_nLastSentOldWeather;
    static int16_t ms_nLastSentNewWeather;
    static int16_t ms_nLastSentForcedWeather;
};
