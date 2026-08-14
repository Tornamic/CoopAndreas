#include "stdafx.h"
#include "CWeatherSync.h"
#include "network/packets/world.h"

static bool s_hasNetState = false;
static bool s_wasDead = false;
static bool s_hasAliveSnapshot = false;

static uint32_t s_lockUntilTick = 0;

static eWeatherType s_newWeather{};
static eWeatherType s_oldWeather{};
static int16_t s_forcedWeather = -1;

static float s_rain = 0.0f;
static float s_wetRoads = 0.0f;
static float s_wind = 0.0f;

static bool s_lightningFlash = false;
static bool s_lightningBurst = false;
static uint32_t s_lightningDuration = 0;
static int32_t s_lightningSoundAt = 0;

static uint8_t s_month = 1;
static uint8_t s_day = 1;
static uint8_t s_hour = 12;
static uint8_t s_minute = 0;
static uint8_t s_second = 0;

static eWeatherType s_aliveNew{};
static eWeatherType s_aliveOld{};
static int16_t s_aliveForced = -1;

static float s_aliveRain = 0.0f;
static float s_aliveWet = 0.0f;
static float s_aliveWind = 0.0f;

static uint8_t s_aliveMonth = 1;
static uint8_t s_aliveDay = 1;
static uint8_t s_aliveHour = 12;
static uint8_t s_aliveMinute = 0;
static uint8_t s_aliveSecond = 0;

static bool s_lastLightningFlash = false;
static bool s_lastLightningBurst = false;

static bool s_remoteLightning = false;
static uint32_t s_remoteLightningUntil = 0;
static uint32_t s_lastLightningSent = 0;

static constexpr uint32_t DEATH_LOCK_MS = 8000;
static constexpr uint32_t LIGHTNING_COOLDOWN_MS = 900;

static bool IsLocalPlayerDown()
{
    CPlayerPed* ped = FindPlayerPed();

    if (!ped)
        return false;

    if (ped->m_fHealth <= 0.0f)
        return true;

    const int state = static_cast<int>(ped->m_ePedState);

    if (state == 55 || state == 54)
        return true;

    if (CWorld::Players[0].m_nPlayerState == PLAYERSTATE_HASDIED)
        return true;

    if (CWorld::Players[0].m_nPlayerState == PLAYERSTATE_HASBEENARRESTED)
        return true;

    return false;
}

static void CaptureAliveSnapshot()
{
    s_aliveNew = static_cast<eWeatherType>(CWeather::NewWeatherType);

    s_aliveOld = static_cast<eWeatherType>(CWeather::OldWeatherType);

    s_aliveForced = CWeather::ForcedWeatherType;

    s_aliveRain = CWeather::Rain;

    s_aliveWet = CWeather::WetRoads;

    s_aliveWind = CWeather::Wind;

    s_aliveMonth = CClock::ms_nGameClockMonth;

    s_aliveDay = CClock::CurrentDay;

    s_aliveHour = CClock::ms_nGameClockHours;

    s_aliveMinute = CClock::ms_nGameClockMinutes;

    s_aliveSecond = CClock::ms_nGameClockSeconds;

    s_hasAliveSnapshot = true;
}

static void ApplyAliveSnapshot()
{
    if (!s_hasAliveSnapshot)
        return;

    CWeather::OldWeatherType = static_cast<short>(s_aliveNew);

    CWeather::NewWeatherType = static_cast<short>(s_aliveNew);

    CWeather::ForcedWeatherType = s_aliveForced;

    CWeather::InterpolationValue = 0.0f;

    CWeather::Rain = s_aliveRain;

    CWeather::WetRoads = s_aliveWet;

    CWeather::Wind = s_aliveWind;

    CClock::ms_nGameClockMonth = s_aliveMonth;

    CClock::CurrentDay = s_aliveDay;

    CClock::ms_nGameClockHours = s_aliveHour;

    CClock::ms_nGameClockMinutes = s_aliveMinute;

    CClock::ms_nGameClockSeconds = s_aliveSecond;
}

void CWeatherSync::ApplyNetStateToLocal()
{
    if (!s_hasNetState)
        return;

    CWeather::OldWeatherType = static_cast<short>(s_newWeather);

    CWeather::NewWeatherType = static_cast<short>(s_newWeather);

    CWeather::ForcedWeatherType = static_cast<short>(s_newWeather);

    CWeather::InterpolationValue = 0.0f;

    CWeather::Rain = s_rain;

    CWeather::WetRoads = s_wetRoads;

    CWeather::Wind = s_wind;

    CWeather::LightningFlash = s_lightningFlash;

    CWeather::LightningBurst = s_lightningBurst;

    CWeather::LightningDuration = s_lightningDuration;

    if (s_lightningSoundAt > 0)
    {
        CWeather::WhenToPlayLightningSound = s_lightningSoundAt;
    }

    CClock::ms_nGameClockMonth = s_month;

    CClock::CurrentDay = s_day;

    CClock::ms_nGameClockHours = s_hour;

    CClock::ms_nGameClockMinutes = s_minute;

    CClock::ms_nGameClockSeconds = s_second;
}

bool CWeatherSync::HasNetState()
{ return s_hasNetState; }

void CWeatherSync::Reset()
{
    s_hasNetState = false;
    s_wasDead = false;
    s_hasAliveSnapshot = false;

    s_lockUntilTick = 0;

    s_newWeather = static_cast<eWeatherType>(CWeather::NewWeatherType);

    s_oldWeather = static_cast<eWeatherType>(CWeather::OldWeatherType);

    s_forcedWeather = CWeather::ForcedWeatherType;

    s_rain = CWeather::Rain;

    s_wetRoads = CWeather::WetRoads;

    s_wind = CWeather::Wind;

    s_lightningFlash = CWeather::LightningFlash;

    s_lightningBurst = CWeather::LightningBurst;

    s_lightningDuration = CWeather::LightningDuration;

    s_lightningSoundAt = 0;

    s_month = CClock::ms_nGameClockMonth;

    s_day = CClock::CurrentDay;

    s_hour = CClock::ms_nGameClockHours;

    s_minute = CClock::ms_nGameClockMinutes;

    s_second = CClock::ms_nGameClockSeconds;

    s_lastLightningFlash = false;
    s_lastLightningBurst = false;

    s_remoteLightning = false;
    s_remoteLightningUntil = 0;
    s_lastLightningSent = 0;
}

void CWeatherSync::SyncCurrentState()
{
    if (!CLocalPlayer::m_bIsHost)
        return;

    if (IsLocalPlayerDown() || GetTickCount() < s_lockUntilTick)
    {
        ApplyAliveSnapshot();
    }

    Packets::World::GameWeatherTime packet{};

    packet.newWeather = static_cast<eWeatherType>(CWeather::NewWeatherType);

    packet.oldWeather = static_cast<eWeatherType>(CWeather::OldWeatherType);

    packet.forcedWeather = CWeather::ForcedWeatherType;

    packet.interpolationValue = CWeather::InterpolationValue;

    packet.rain = CWeather::Rain;

    packet.wetRoads = CWeather::WetRoads;

    packet.wind = CWeather::Wind;

    packet.lightningFlash = CWeather::LightningFlash;

    packet.lightningBurst = CWeather::LightningBurst;

    packet.lightningDuration = CWeather::LightningDuration;

    if (CWeather::WhenToPlayLightningSound > (int)CTimer::m_snTimeInMilliseconds)
    {
        packet.lightningSoundDelay = CWeather::WhenToPlayLightningSound - (int)CTimer::m_snTimeInMilliseconds;
    }
    else
    {
        packet.lightningSoundDelay = -1;
    }

    packet.currentMonth = CClock::ms_nGameClockMonth;

    packet.currentDay = CClock::CurrentDay;

    packet.currentHour = CClock::ms_nGameClockHours;

    packet.currentMinute = CClock::ms_nGameClockMinutes;

    packet.currentSecond = CClock::ms_nGameClockSeconds;

    GetPacketFactory().Send(packet);
}

void CWeatherSync::HandlePacket(Packets::World::GameWeatherTime* p)
{
    if (!p || CLocalPlayer::m_bIsHost)
        return;

    s_newWeather = p->newWeather;

    s_oldWeather = p->oldWeather;

    s_forcedWeather = p->forcedWeather;

    s_rain = p->rain;

    s_wetRoads = p->wetRoads;

    s_wind = p->wind;

    s_lightningFlash = p->lightningFlash;

    s_lightningBurst = p->lightningBurst;

    s_lightningDuration = p->lightningDuration;

    if (p->lightningSoundDelay >= 0)
    {
        s_lightningSoundAt = (int)CTimer::m_snTimeInMilliseconds + p->lightningSoundDelay;
    }
    else
    {
        s_lightningSoundAt = 0;
    }

    s_month = p->currentMonth;

    s_day = p->currentDay;

    s_hour = p->currentHour;

    s_minute = p->currentMinute;

    s_second = p->currentSecond;

    s_hasNetState = true;

    const uint32_t now = GetTickCount();

    uint32_t lightningLock = p->lightningDuration;

    if (lightningLock < 500)
    {
        lightningLock = 500;
    }

    s_remoteLightning = true;

    s_remoteLightningUntil = now + lightningLock + 500;

    s_lastLightningFlash = p->lightningFlash;

    s_lastLightningBurst = p->lightningBurst;

    ApplyNetStateToLocal();
}

void CWeatherSync::HandleLightningEvent(Packets::World::LightningEvent* p)
{
    if (!p)
        return;

    CWeather::LightningFlash = p->flash;

    CWeather::LightningBurst = p->burst;

    CWeather::LightningDuration = p->duration;

    if (p->soundDelay >= 0)
    {
        CWeather::WhenToPlayLightningSound = (int)CTimer::m_snTimeInMilliseconds + p->soundDelay;
    }

    uint32_t duration = p->duration;

    if (duration < 500)
    {
        duration = 500;
    }

    s_remoteLightning = true;

    s_remoteLightningUntil = GetTickCount() + duration + 500;

    s_lastLightningFlash = p->flash;

    s_lastLightningBurst = p->burst;
}

void CWeatherSync::Process()
{
    if (!CNetwork::m_bAuthenticated)
        return;

    const uint32_t now = GetTickCount();

    if (CLocalPlayer::m_bIsHost)
    {
        const bool down = IsLocalPlayerDown();

        if (down)
        {
            if (!s_wasDead)
            {
                CaptureAliveSnapshot();
            }

            if (s_hasAliveSnapshot)
            {
                ApplyAliveSnapshot();
            }

            s_lockUntilTick = now + DEATH_LOCK_MS;

            s_wasDead = true;

            return;
        }

        if (s_wasDead)
        {
            if (s_hasAliveSnapshot)
            {
                ApplyAliveSnapshot();
            }

            s_lockUntilTick = now + DEATH_LOCK_MS;

            s_wasDead = false;

            return;
        }

        if (now >= s_lockUntilTick)
        {
            CaptureAliveSnapshot();
        }

        return;
    }

    if (s_remoteLightning && now >= s_remoteLightningUntil)
    {
        s_remoteLightning = false;
    }

    const bool flash = CWeather::LightningFlash;

    const bool burst = CWeather::LightningBurst;

    const bool lightningStarted = (!s_lastLightningFlash && flash) || (!s_lastLightningBurst && burst);

    if (!s_remoteLightning && lightningStarted && now - s_lastLightningSent >= LIGHTNING_COOLDOWN_MS)
    {
        Packets::World::LightningEvent packet{};

        packet.flash = flash;

        packet.burst = burst;

        packet.duration = CWeather::LightningDuration;

        if (CWeather::WhenToPlayLightningSound > (int)CTimer::m_snTimeInMilliseconds)
        {
            packet.soundDelay = CWeather::WhenToPlayLightningSound - (int)CTimer::m_snTimeInMilliseconds;
        }
        else
        {
            packet.soundDelay = -1;
        }

        GetPacketFactory().Send(packet);

        s_lastLightningSent = now;
    }

    s_lastLightningFlash = flash;

    s_lastLightningBurst = burst;
}

void CWeatherSync::ServerTimeRecalculated(server_time_t serverTime)
{ CWaterLevel::m_nWaterTimeOffset = CTimer::m_snTimeInMilliseconds - serverTime; }