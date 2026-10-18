#include "CWeatherSync.h"

// the host resends the state this often, and at once when its weather changes
constexpr uint32_t SYNC_INTERVAL_MS = 1000;

bool CWeatherSync::ms_bHasHostState = false;
int16_t CWeatherSync::ms_nHostNewWeather = -1;
int16_t CWeatherSync::ms_nHostForcedWeather = -1;

uint32_t CWeatherSync::ms_nLastSyncTime = 0;
int16_t CWeatherSync::ms_nLastSentOldWeather = -1;
int16_t CWeatherSync::ms_nLastSentNewWeather = -1;
int16_t CWeatherSync::ms_nLastSentForcedWeather = -1;

void CWeatherSync::Process()
{
    if (CLocalPlayer::m_bIsHost)
    {
        if (CWeather::OldWeatherType != ms_nLastSentOldWeather || CWeather::NewWeatherType != ms_nLastSentNewWeather ||
            CWeather::ForcedWeatherType != ms_nLastSentForcedWeather ||
            GetTickCount() - ms_nLastSyncTime >= SYNC_INTERVAL_MS)
        {
            SyncCurrentState();
        }
        return;
    }

    if (ms_bHasHostState)
    {
        // with a forced weather the hour change in CWeather::Update does Old = New, New = Forced instead of advancing
        // the region's weather list, so this player keeps the host's weather until the host sends the next one.
        // Set every frame: SetWeatherToAppropriateTypeNow, ReleaseWeather and the weather cheats reset it
        CWeather::ForcedWeatherType = ms_nHostNewWeather;
    }
}

void CWeatherSync::SyncCurrentState()
{
    if (!CLocalPlayer::m_bIsHost)
    {
        return;
    }

    const uint32_t msPerGameMinute = std::max(CClock::ms_nMillisecondsPerGameMinute, 1u);
    const uint32_t minuteElapsedMs = CTimer::m_snTimeInMilliseconds - CClock::ms_nLastClockTick;

    Packets::World::GameWeatherTime packet{};
    packet.newWeather = static_cast<eWeatherType>(CWeather::NewWeatherType);
    packet.oldWeather = static_cast<eWeatherType>(CWeather::OldWeatherType);
    packet.forcedWeather = CWeather::ForcedWeatherType;
    packet.currentMonth = CClock::ms_nGameClockMonth;
    packet.currentMonthDay = CClock::ms_nGameClockDays;
    packet.currentDay = CClock::CurrentDay;
    packet.currentHour = CClock::ms_nGameClockHours;
    packet.currentMinute = CClock::ms_nGameClockMinutes;
    packet.minuteElapsedMs = std::min(minuteElapsedMs, msPerGameMinute);
    packet.msPerGameMinute = msPerGameMinute;
    GetPacketFactory().Send(packet);

    ms_nLastSyncTime = GetTickCount();
    ms_nLastSentOldWeather = CWeather::OldWeatherType;
    ms_nLastSentNewWeather = CWeather::NewWeatherType;
    ms_nLastSentForcedWeather = CWeather::ForcedWeatherType;
}

void CWeatherSync::HandlePacket(Packets::World::GameWeatherTime* pGameWeatherTime)
{
    if (CLocalPlayer::m_bIsHost)
    {
        return;
    }

    ms_bHasHostState = true;
    ms_nHostNewWeather = static_cast<int16_t>(pGameWeatherTime->newWeather);
    ms_nHostForcedWeather = pGameWeatherTime->forcedWeather;

    CWeather::OldWeatherType = static_cast<int16_t>(pGameWeatherTime->oldWeather);
    CWeather::NewWeatherType = ms_nHostNewWeather;
    CWeather::ForcedWeatherType = ms_nHostNewWeather;

    const uint32_t msPerGameMinute = std::max(pGameWeatherTime->msPerGameMinute, 1u);
    const uint32_t minuteElapsedMs = std::min(pGameWeatherTime->minuteElapsedMs, msPerGameMinute);

    CClock::ms_nMillisecondsPerGameMinute = msPerGameMinute;
    CClock::ms_nGameClockMonth = pGameWeatherTime->currentMonth;
    CClock::ms_nGameClockDays = pGameWeatherTime->currentMonthDay;
    CClock::CurrentDay = pGameWeatherTime->currentDay;
    CClock::ms_nGameClockHours = pGameWeatherTime->currentHour;
    CClock::ms_nGameClockMinutes = pGameWeatherTime->currentMinute;
    // keep the host's progress through the minute, CClock::Update counts the seconds and the next minute from it
    CClock::ms_nLastClockTick = CTimer::m_snTimeInMilliseconds - minuteElapsedMs;
    CClock::ms_nGameClockSeconds = static_cast<uint16_t>(std::min(minuteElapsedMs * 60 / msPerGameMinute, 59u));

    // CWeather::Update takes a drop of this value as the top of the hour and shifts the weather. Our clock may have
    // just been moved over the hour either way, so line it up with the new time: Old/New are already the host's
    CWeather::InterpolationValue =
        (static_cast<float>(CClock::ms_nGameClockSeconds) / 60.0f + CClock::ms_nGameClockMinutes) / 60.0f;
}

void CWeatherSync::ServerTimeRecalculated(server_time_t serverTime)
{
    CWaterLevel::m_nWaterTimeOffset = CTimer::m_snTimeInMilliseconds - serverTime;
}

void CWeatherSync::OnBecameHost()
{
    Reset();
    SyncCurrentState();
}

void CWeatherSync::Reset()
{
    if (ms_bHasHostState)
    {
        // give the weather back to the game the way the host had it
        CWeather::ForcedWeatherType = ms_nHostForcedWeather;
        ms_bHasHostState = false;
    }
}
