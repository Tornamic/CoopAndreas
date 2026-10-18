#include "stdafx.h"
#include "CTrafficLightsSync.h"
#include <CGameLogic.h>
#include <CCheat.h>
#include <CServerTime.h>

// the phase of every traffic light in the game comes from one clock: (time / 2) & 16383, a 32.8 s cycle. It was the
// local game time, different on each client, so the traffic of one player stopped on its red while the other one
// saw green. Online it is the server time all clients share
static uint32_t GetTrafficLightTime()
{
    if (CNetwork::m_bAuthenticated)
    {
        return g_serverTime;
    }
    return CTimer::m_snTimeInMilliseconds;
}

static uint32_t GetPhase()
{
    return (GetTrafficLightTime() / 2) & 16383;
}

static bool AreAllLightsGreen()
{
    constexpr int CHEAT_GREEN_LIGHTS = 30;  // eCheats CHEAT_BGREEN_LIGHTS_CHEAT
    return CGameLogic::LaRiotsActiveHere() || CCheat::m_aCheatsActive[CHEAT_GREEN_LIGHTS];
}

// CTrafficLights::LightForPeds (0x49D400)
static uint8_t __cdecl CTrafficLights__LightForPeds_Hook()
{
    uint32_t phase = GetPhase();
    if (phase < 12000)
    {
        return CTrafficLightsSync::LIGHT_RED;
    }
    return phase < 16383 - 1000 ? CTrafficLightsSync::LIGHT_GREEN : CTrafficLightsSync::LIGHT_YELLOW;
}

// CTrafficLights::LightForCars1 (0x49D2D0): north - south
static uint8_t __cdecl CTrafficLights__LightForCars1_Hook()
{
    if (AreAllLightsGreen())
    {
        return CTrafficLightsSync::LIGHT_GREEN;
    }

    uint32_t phase = GetPhase();
    if (phase < 5000)
    {
        return CTrafficLightsSync::LIGHT_GREEN;
    }
    return phase < 6000 ? CTrafficLightsSync::LIGHT_YELLOW : CTrafficLightsSync::LIGHT_RED;
}

// CTrafficLights::LightForCars2 (0x49D310): west - east
static uint8_t __cdecl CTrafficLights__LightForCars2_Hook()
{
    if (AreAllLightsGreen())
    {
        return CTrafficLightsSync::LIGHT_GREEN;
    }

    uint32_t phase = GetPhase();
    if (phase < 6000)
    {
        return CTrafficLightsSync::LIGHT_RED;
    }
    if (phase < 11000)
    {
        return CTrafficLightsSync::LIGHT_GREEN;
    }
    return phase < 12000 ? CTrafficLightsSync::LIGHT_YELLOW : CTrafficLightsSync::LIGHT_RED;
}

// the lamps blink yellow in the riots
static uint8_t GetVisualLight(uint8_t (*pfnLightForCars)())
{
    if (CGameLogic::LaRiotsActiveHere())
    {
        return ((GetTrafficLightTime() >> 9) & 2) | 1;
    }
    return pfnLightForCars();
}

// CTrafficLights::LightForCars1_Visual (0x49D350)
static uint8_t __cdecl CTrafficLights__LightForCars1_Visual_Hook()
{
    return GetVisualLight(CTrafficLights__LightForCars1_Hook);
}

// CTrafficLights::LightForCars2_Visual (0x49D3A0)
static uint8_t __cdecl CTrafficLights__LightForCars2_Visual_Hook()
{
    return GetVisualLight(CTrafficLights__LightForCars2_Hook);
}

// CTrafficLights::DisplayActualLight inlines LightForPeds for the walk lamps and keeps the time in ecx for their
// blinking (test ch, 1 further on): load the shared time there instead of [CTimer::m_snTimeInMilliseconds]
static void __declspec(naked) CTrafficLights__DisplayActualLight_LoadTime()
{
    __asm
    {
        push eax
        push edx
        call GetTrafficLightTime
        mov ecx, eax
        pop edx
        pop eax
        ret
    }
}

void CTrafficLightsSync::InjectHooks()
{
    // whole functions (the car ones are HOODLUM jump chains, there is no single time read to patch): the ai goes
    // through them (CTrafficLights::ShouldCarStopForLight), peds crossing too
    patch::RedirectJump(0x49D400, CTrafficLights__LightForPeds_Hook);
    patch::RedirectJump(0x49D2D0, CTrafficLights__LightForCars1_Hook);
    patch::RedirectJump(0x49D310, CTrafficLights__LightForCars2_Hook);

    // the lamps have their own copies of the formula on the local time, so the ai stopped on a light that was drawn
    // green
    patch::RedirectJump(0x49D350, CTrafficLights__LightForCars1_Visual_Hook);
    patch::RedirectJump(0x49D3A0, CTrafficLights__LightForCars2_Visual_Hook);

    // mov ecx, [0xB7CB84] (6 bytes) -> call + nop
    patch::RedirectCall(0x49E1D9, CTrafficLights__DisplayActualLight_LoadTime);
    patch::Nop(0x49E1D9 + 5, 1);
}
