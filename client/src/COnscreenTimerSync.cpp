#include "stdafx.h"
#include "COnscreenTimerSync.h"
#include "COpCodeSync.h"
#include "Hooks/CHook.h"
#include <CUserDisplay.h>

static const unsigned int COUNTER_SEND_INTERVAL = 100;
static const unsigned int PERIODIC_SEND_INTERVAL = 1000;
static const int CLOCK_CORRECTION_THRESHOLD = 250;

static Packets::Scripts::OnscreenTimerSync lastSentState{};
static unsigned int lastSendTime = 0;

static int& GetScriptVar(unsigned int nIndex)
{ return *reinterpret_cast<int*>(&CTheScripts::ScriptSpace[nIndex]); }

static bool IsCreatedBySyncedMission()
{
    CRunningScript* script = COpCodeSync::GetLastProcessedScript();
    return script != nullptr && script->m_bIsMission && COpCodeSync::IsScriptSynced(script);
}

static void __fastcall COnscreenTimer__AddClock_Hook(
    COnscreenTimer* This, SKIP_EDX, unsigned int varId, char* gxt, bool bTimerDirection)
{
    This->AddClock(varId, gxt, bTimerDirection);

    if (This->m_Clock.m_nVarId == varId)
    {
        COnscreenTimerSync::ms_bClockNetwork = IsCreatedBySyncedMission();
    }
}

static void __fastcall COnscreenTimer__AddCounter_Hook(
    COnscreenTimer* This, SKIP_EDX, int varId, short type, char* gxt, unsigned short counterIndex)
{
    This->AddCounter(varId, type, gxt, counterIndex);

    if (This->m_aCounters[counterIndex].m_nVarId == varId)
    {
        COnscreenTimerSync::ms_abCounterNetwork[counterIndex] = IsCreatedBySyncedMission();
    }
}

static void BuildPacket(Packets::Scripts::OnscreenTimerSync& packet)
{
    COnscreenTimer& timer = CUserDisplay::OnscnTimer;

    if (timer.m_Clock.m_nVarId != 0 && COnscreenTimerSync::ms_bClockNetwork)
    {
        auto& clock = packet.clock;
        clock.bActive = true;
        clock.varIndex = timer.m_Clock.m_nVarId;
        clock.value = GetScriptVar(clock.varIndex);
        clock.bCountDown = timer.m_Clock.m_nTimerDirection != 0;
        clock.beepSecs = timer.m_Clock.m_nClockBeepCountdownSecs;
        strncpy_s(clock.textKey, timer.m_Clock.m_szDescriptionTextKey, _TRUNCATE);
    }

    for (int i = 0; i < 4; i++)
    {
        const COnscreenCounterEntry& entry = timer.m_aCounters[i];
        if (entry.m_nVarId == 0 || !COnscreenTimerSync::ms_abCounterNetwork[i])
        {
            continue;
        }

        auto& counter = packet.counters[i];
        counter.bActive = true;
        counter.varIndex = entry.m_nVarId;
        counter.value = GetScriptVar(counter.varIndex);
        counter.type = entry.m_nType;
        if (counter.type == 2)
        {
            counter.varIndex2 = entry.m_nMaxVarValue;
            counter.value2 = GetScriptVar(counter.varIndex2);
        }
        counter.bFlash = entry.m_bFlashWhenFirstDisplayed;
        counter.colour = entry.m_nColourId;
        strncpy_s(counter.textKey, entry.m_szDescriptionTextKey, _TRUNCATE);
    }

    packet.bPaused = timer.m_bPaused;
}

// the clock value is left out, it changes every frame and is resent periodically
static bool IsSameState(const Packets::Scripts::OnscreenTimerSync& a, const Packets::Scripts::OnscreenTimerSync& b)
{
    if (a.clock.bActive != b.clock.bActive || a.clock.varIndex != b.clock.varIndex ||
        a.clock.bCountDown != b.clock.bCountDown || a.clock.beepSecs != b.clock.beepSecs ||
        strcmp(a.clock.textKey, b.clock.textKey) != 0 || a.bPaused != b.bPaused)
    {
        return false;
    }

    for (int i = 0; i < 4; i++)
    {
        const auto& x = a.counters[i];
        const auto& y = b.counters[i];
        if (x.bActive != y.bActive || x.varIndex != y.varIndex || x.value != y.value || x.varIndex2 != y.varIndex2 ||
            x.value2 != y.value2 || x.type != y.type || x.bFlash != y.bFlash || x.colour != y.colour ||
            strcmp(x.textKey, y.textKey) != 0)
        {
            return false;
        }
    }

    return true;
}

static bool IsAnythingActive(const Packets::Scripts::OnscreenTimerSync& packet)
{
    for (int i = 0; i < 4; i++)
    {
        if (packet.counters[i].bActive)
        {
            return true;
        }
    }

    return packet.clock.bActive;
}

void COnscreenTimerSync::Process()
{
    Packets::Scripts::OnscreenTimerSync packet{};
    BuildPacket(packet);

    unsigned int nElapsed = GetTickCount() - lastSendTime;
    if ((!IsSameState(packet, lastSentState) && nElapsed >= COUNTER_SEND_INTERVAL) ||
        (IsAnythingActive(packet) && nElapsed >= PERIODIC_SEND_INTERVAL))
    {
        GetPacketFactory().Send(packet);
        lastSentState = packet;
        lastSendTime = GetTickCount();
    }
}

void COnscreenTimerSync::SendState()
{
    Packets::Scripts::OnscreenTimerSync packet{};
    BuildPacket(packet);
    GetPacketFactory().Send(packet);
    lastSentState = packet;
    lastSendTime = GetTickCount();
}

void COnscreenTimerSync::ClearNetworkEntries()
{
    COnscreenTimer& timer = CUserDisplay::OnscnTimer;

    if (ms_bClockNetwork)
    {
        timer.ClearClock(timer.m_Clock.m_nVarId);
        timer.m_bPaused = false;
        ms_bClockNetwork = false;
    }

    for (int i = 0; i < 4; i++)
    {
        if (ms_abCounterNetwork[i])
        {
            timer.ClearCounter(timer.m_aCounters[i].m_nVarId);
            ms_abCounterNetwork[i] = false;
        }
    }
}

void COnscreenTimerSync::HandlePacket(const Packets::Scripts::OnscreenTimerSync& packet)
{
    if (CLocalPlayer::m_bIsHost)
    {
        return;
    }

    COnscreenTimer& timer = CUserDisplay::OnscnTimer;

    if (packet.clock.bActive)
    {
        COnscreenTimerEntry& clock = timer.m_Clock;
        int& value = GetScriptVar(packet.clock.varIndex);

        // the clock keeps running locally between packets, only big differences are corrected
        if (!ms_bClockNetwork || clock.m_nVarId != packet.clock.varIndex ||
            abs(value - packet.clock.value) > CLOCK_CORRECTION_THRESHOLD)
        {
            value = packet.clock.value;
        }

        clock.m_nVarId = packet.clock.varIndex;
        clock.m_nTimerDirection = packet.clock.bCountDown;
        clock.m_nClockBeepCountdownSecs = packet.clock.beepSecs;
        strncpy_s(clock.m_szDescriptionTextKey, packet.clock.textKey, _TRUNCATE);
        timer.m_bPaused = packet.bPaused;
        ms_bClockNetwork = true;
    }
    else if (ms_bClockNetwork)
    {
        timer.ClearClock(timer.m_Clock.m_nVarId);
        timer.m_bPaused = false;
        ms_bClockNetwork = false;
    }

    for (int i = 0; i < 4; i++)
    {
        const auto& counter = packet.counters[i];
        COnscreenCounterEntry& entry = timer.m_aCounters[i];

        if (counter.bActive)
        {
            GetScriptVar(counter.varIndex) = counter.value;
            if (counter.type == 2)
            {
                GetScriptVar(counter.varIndex2) = counter.value2;
            }

            entry.m_nVarId = counter.varIndex;
            entry.m_nMaxVarValue = counter.type == 2 ? counter.varIndex2 : 0;
            entry.m_nType = counter.type;
            entry.m_bFlashWhenFirstDisplayed = counter.bFlash;
            entry.m_nColourId = counter.colour;
            strncpy_s(entry.m_szDescriptionTextKey, counter.textKey, _TRUNCATE);
            ms_abCounterNetwork[i] = true;
        }
        else if (ms_abCounterNetwork[i])
        {
            timer.ClearCounter(entry.m_nVarId);
            ms_abCounterNetwork[i] = false;
        }
    }
}

void COnscreenTimerSync::InjectHooks()
{
    patch::RedirectCall({0x47C2AC, 0x47C2C3, 0x484CA7}, COnscreenTimer__AddClock_Hook);
    patch::RedirectCall({0x484CFC, 0x48C57D}, COnscreenTimer__AddCounter_Hook);
}
