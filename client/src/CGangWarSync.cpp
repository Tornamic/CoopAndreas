#include "stdafx.h"
#include "CGangWarSync.h"
#include "CGangZonesSync.h"
#include <CGangWars.h>
#include <CGeneral.h>
#include <CRadar.h>

using namespace Packets::World;

void __cdecl CGangWarSync::CGangWars__Update_Hook()
{
    // the war of someone else runs on its owner, ours would start another one and spawn its own waves
    if (!ms_bMirroring)
    {
        plugin::Call<0x446610>();
    }
}

void __cdecl CGangWarSync::CGangWars__StartDefensiveGangWar_Hook()
{
    // the enemy attacks come from the host's timer only, otherwise every player would get its own
    if (CNetwork::m_bAuthenticated && !CLocalPlayer::m_bIsHost)
    {
        CGangWars::TimeTillNextAttack = CGeneral::GetRandomNumberInRange(648000.0f, 1620000.0f);
        return;
    }
    plugin::Call<0x444300>();
}

void __cdecl CGangWarSync::CGangWars__DoStuffWhenPlayerVictorious_Hook()
{
    ms_bVictory = true;
    plugin::Call<0x446400>();
}

void CGangWarSync::InjectHooks()
{
    patch::RedirectCall(0x53C122, CGangWars__Update_Hook);                     // CGame::Process
    patch::RedirectCall(0x446E46, CGangWars__StartDefensiveGangWar_Hook);      // CGangWars::Update, the attack timer
    patch::RedirectCall(0x44690B, CGangWars__DoStuffWhenPlayerVictorious_Hook); // CGangWars::Update, third wave beaten
}

void CGangWarSync::Reset()
{
    if (ms_bMirroring)
    {
        EndMirror(GangWarState::RESULT_ABORTED);
    }

    ms_bWasRunning = false;
    ms_bVictory = false;
    ms_nOwnedZoneInfo = -1;
    ms_lastSent = GangWarState{};
}

bool CGangWarSync::IsLocalWarRunning()
{
    return !ms_bMirroring && (CGangWars::State != NOT_IN_WAR || CGangWars::State2 != NO_ATTACK);
}

unsigned int CGangWarSync::GetGangColour(int gang)
{
    // the gang colours as CGangWars::CreateAttackWave colours the blips of the attackers (gta-reversed swaps the names
    // of the G and B arrays: Ballas are 200,0,200 with 0x8D1350 as G)
    gang = std::clamp(gang, 0, GANG_COUNT - 1);
    unsigned int r = *reinterpret_cast<uint8_t*>(0x8D1344 + gang);
    unsigned int g = *reinterpret_cast<uint8_t*>(0x8D1350 + gang);
    unsigned int b = *reinterpret_cast<uint8_t*>(0x8D135C + gang);
    return (r << 24) | (g << 16) | (b << 8) | 0xFF;
}

bool CGangWarSync::IsNearWarZone(const CVector& position)
{
    // the area CGangWars::Update counts as being at the war
    CZone* pZone = CGangWars::pZoneToFightOver;
    return pZone != nullptr && pZone->m_fX1 - 50 <= position.x && pZone->m_fX2 + 50 >= position.x &&
           pZone->m_fY1 - 50 <= position.y && pZone->m_fY2 + 50 >= position.y;
}

void CGangWarSync::ShowText(const char* gxt, bool bHelp)
{
    char key[8]{};
    strncpy_s(key, gxt, _TRUNCATE);
    if (bHelp)
    {
        Command<Commands::PRINT_HELP>(key);
    }
    else
    {
        Command<Commands::PRINT>(key, 4500, 1);
    }
}

GangWarState CGangWarSync::Collect()
{
    GangWarState state{};
    state.active = CGangWars::State != NOT_IN_WAR || CGangWars::State2 != NO_ATTACK;
    state.state = static_cast<uint8_t>(std::clamp<int>(CGangWars::State, 0, 6));
    state.state2 = static_cast<uint8_t>(std::clamp<int>(CGangWars::State2, 0, 2));
    state.gang = static_cast<uint8_t>(std::clamp(CGangWars::Gang1, 0, GANG_COUNT - 1));
    state.zone = static_cast<int16_t>(CGangZonesSync::GetNavigationZoneIndex(CGangWars::pZoneToFightOver));
    state.pointOfAttack = CGangWars::PointOfAttack;

    if (!state.active)
    {
        return state;
    }

    for (int i = 0; i < CPools::ms_pPedPool->m_nSize && state.pedCount < GangWarState::MAX_WAVE_PEDS; i++)
    {
        CPed* pPed = CPools::ms_pPedPool->GetAt(i);
        if (pPed == nullptr || !pPed->m_nPedFlags.bPartOfAttackWave)
        {
            continue;
        }

        if (CNetworkPed* pNetworkPed = CNetworkPedManager::GetPed(pPed))
        {
            state.peds[state.pedCount++] = pNetworkPed->m_pedId;
        }
    }
    return state;
}

uint8_t CGangWarSync::GetEndResult()
{
    if (ms_bVictory)
    {
        return GangWarState::RESULT_VICTORY;
    }

    // CGangWars::Update leaves PLAYER_CAME_TO_WAR only by beating the attackers or running out of time, the
    // notified attack only by running out of time or by EndGangWar
    if (ms_lastSent.state2 == PLAYER_CAME_TO_WAR)
    {
        return CGangWars::FightTimer < 0 ? GangWarState::RESULT_TOO_SLOW : GangWarState::RESULT_DEFENDED;
    }
    if (ms_lastSent.state2 == WAR_NOTIFIED)
    {
        return CGangWars::FightTimer < 0 ? GangWarState::RESULT_TOO_LATE : GangWarState::RESULT_ABORTED;
    }
    if (CTimer::m_snTimeInMilliseconds - CGangWars::LastTimeInArea > 30000)
    {
        return GangWarState::RESULT_FLED;
    }
    return GangWarState::RESULT_ABORTED;
}

void CGangWarSync::KeepOwnerInArea()
{
    // the war is lost when its owner is away from the zone for 30 s; another player fighting there keeps it going
    if (CGangWars::State == NOT_IN_WAR)
    {
        return;
    }

    for (auto pNetworkPlayer : CNetworkPlayerManager::m_pPlayers)
    {
        if (pNetworkPlayer->m_pPed && IsNearWarZone(pNetworkPlayer->m_pPed->GetPosition()))
        {
            CGangWars::LastTimeInArea = CTimer::m_snTimeInMilliseconds;
            return;
        }
    }
}

void CGangWarSync::PreProcess()
{
    if (IsLocalWarRunning())
    {
        int nZoneInfo = static_cast<int>(CGangWars::pZoneInfoToFightOver - CGangZonesSync::GetZoneInfos());
        ms_nOwnedZoneInfo = CGangWars::pZoneInfoToFightOver != nullptr && nZoneInfo >= 0 &&
                                    nZoneInfo < CGangZonesSync::GetZoneInfoCount()
                                ? nZoneInfo
                                : -1;
    }
}

void CGangWarSync::Process()
{
    if (ms_bMirroring)
    {
        // scripts and our own code may touch them, the war of the owner holds
        ApplyMirror();
        UpdateWaveBlips();
        return;
    }

    bool bRunning = IsLocalWarRunning();
    if (bRunning || ms_bWasRunning)
    {
        GangWarState state = Collect();
        if (!bRunning)
        {
            state.result = GetEndResult();
        }

        if (!state.IsSameWar(ms_lastSent))
        {
            GetPacketFactory().Send(state);
            ms_lastSent = state;
        }
    }

    if (bRunning)
    {
        KeepOwnerInArea();
    }
    else
    {
        // after CGangZonesSync::Process has sent the last change of the zone
        ms_nOwnedZoneInfo = -1;
        ms_bVictory = false;
        ms_lastSent = GangWarState{};
    }
    ms_bWasRunning = bRunning;
}

void CGangWarSync::ApplyMirror()
{
    CGangWars::State = static_cast<eGangWarState>(ms_mirror.state);
    CGangWars::State2 = static_cast<eGangAttackState>(ms_mirror.state2);
    CGangWars::Gang1 = ms_mirror.gang;
    CGangWars::PointOfAttack = ms_mirror.pointOfAttack;

    CZone* pZone = CGangZonesSync::GetNavigationZone(ms_mirror.zone);
    if (pZone != nullptr)
    {
        CGangWars::pZoneToFightOver = pZone;
        CGangWars::pZoneInfoToFightOver = CGangZonesSync::GetZoneInfo(pZone);
    }

    // our own provocation from the kills in someone else's war would start a new one right after it
    CGangWars::Provocation = 0.0f;
    UpdateAttackBlip();
}

void CGangWarSync::UpdateAttackBlip()
{
    bool bNeeded = ms_bMirroring && ms_mirror.state2 == WAR_NOTIFIED;
    if (bNeeded && ms_nAttackBlip == -1)
    {
        // as StartDefensiveGangWar marks the attacked zone
        unsigned int colour = GetGangColour(ms_mirror.gang);
        ms_nAttackBlip =
            CRadar::SetCoordBlip(BLIP_COORD, ms_mirror.pointOfAttack, colour, BLIP_DISPLAY_BLIP_ONLY, nullptr);
        int sprite = RADAR_SPRITE_ENEMYATTACK;
        switch (ms_mirror.gang)
        {
            case 0: // GANG_BALLAS
                sprite = RADAR_SPRITE_GANGP;
                break;
            case 2: // GANG_VAGOS
            case 4: // GANG_DANANGBOYS
                sprite = RADAR_SPRITE_GANGY;
                break;
            case 3: // GANG_RIFA
                sprite = RADAR_SPRITE_GANGB;
                break;
        }
        CRadar::SetBlipSprite(ms_nAttackBlip, sprite);
    }
    else if (!bNeeded && ms_nAttackBlip != -1)
    {
        CRadar::ClearBlip(ms_nAttackBlip);
        ms_nAttackBlip = -1;
    }
}

void CGangWarSync::UpdateWaveBlips()
{
    // the attackers of the owner, its CreateAttackWave gives them the blips there
    for (auto it = ms_waveBlips.begin(); it != ms_waveBlips.end();)
    {
        CNetworkPed* pNetworkPed = CNetworkPedManager::GetPed(it->pedid);
        bool bInWave = std::find(ms_mirror.peds, ms_mirror.peds + ms_mirror.pedCount, it->pedid) !=
                       ms_mirror.peds + ms_mirror.pedCount;
        if (bInWave && pNetworkPed && pNetworkPed->m_pPed == it->pPed)
        {
            ++it;
            continue;
        }

        CRadar::ClearBlip(it->nHandle);
        it = ms_waveBlips.erase(it);
    }

    for (int i = 0; i < ms_mirror.pedCount; i++)
    {
        pedid_t pedid = ms_mirror.peds[i];
        if (std::any_of(ms_waveBlips.begin(), ms_waveBlips.end(), [&](const SWaveBlip& blip) { return blip.pedid == pedid; }))
        {
            continue;
        }

        CNetworkPed* pNetworkPed = CNetworkPedManager::GetPed(pedid);
        if (pNetworkPed == nullptr || pNetworkPed->m_pPed == nullptr)
        {
            continue;
        }

        unsigned int colour = GetGangColour(ms_mirror.gang);
        int nHandle = CRadar::SetEntityBlip(BLIP_CHAR, CPools::GetPedRef(pNetworkPed->m_pPed), colour, BLIP_DISPLAY_BLIP_ONLY);
        CRadar::ChangeBlipScale(nHandle, 2);
        CRadar::ChangeBlipColour(nHandle, colour);
        ms_waveBlips.push_back({pedid, pNetworkPed->m_pPed, nHandle});
    }
}

void CGangWarSync::ClearWaveBlips()
{
    for (auto& blip : ms_waveBlips)
    {
        CRadar::ClearBlip(blip.nHandle);
    }
    ms_waveBlips.clear();
}

void CGangWarSync::EndMirror(uint8_t result)
{
    switch (result)
    {
        case GangWarState::RESULT_VICTORY:
            ShowText("GW_YRS");
            // the respect DoStuffWhenPlayerVictorious gives the owner, for having fought there too
            if (CPlayerPed* pPlayer = FindPlayerPed(0); pPlayer && IsNearWarZone(pPlayer->GetPosition()))
            {
                CStats::IncrementStat(STAT_RESPECT, 45.0f);
            }
            break;
        case GangWarState::RESULT_FLED:
            ShowText("GW_FLEE");
            break;
        case GangWarState::RESULT_DEFENDED:
            ShowText("GW_WON");
            break;
        case GangWarState::RESULT_TOO_LATE:
            ShowText("GW_NOSH");
            break;
        case GangWarState::RESULT_TOO_SLOW:
            ShowText("GW_SLOW");
            break;
    }

    ms_bMirroring = false;
    ms_mirror = GangWarState{};
    ClearWaveBlips();
    UpdateAttackBlip();

    CGangWars::State = NOT_IN_WAR;
    CGangWars::State2 = NO_ATTACK;
    CGangWars::Provocation = 0.0f;
    CGangZonesSync::RefreshColours();
}

void CGangWarSync::HandlePacket(GangWarState* pState)
{
    if (!pState->active)
    {
        if (ms_bMirroring && pState->owner == ms_mirror.owner)
        {
            EndMirror(pState->result);
        }
        return;
    }

    if (IsLocalWarRunning())
    {
        // the server took the other war first and dropped ours
        plugin::Call<0x4464C0, bool>(false); // CGangWars::EndGangWar
        ms_bWasRunning = false;
        ms_bVictory = false;
        ms_nOwnedZoneInfo = -1;
        ms_lastSent = GangWarState{};
    }

    GangWarState previous = ms_bMirroring ? ms_mirror : GangWarState{};
    ms_bMirroring = true;
    ms_mirror = *pState;

    // the texts CGangWars::Update shows on these transitions
    if (previous.state == NOT_IN_WAR && pState->state == PREFIRST_WAVE)
    {
        ShowText("GW_PROV");
    }
    else if (previous.state == FIRST_WAVE && pState->state == PRESECOND_WAVE)
    {
        ShowText("GW_CLR1");
    }
    else if (previous.state == SECOND_WAVE && pState->state == PRETHIRD_WAVE)
    {
        ShowText("GW_CLR2");
    }
    if (previous.state2 == NO_ATTACK && pState->state2 == WAR_NOTIFIED)
    {
        ShowText("GW_ATK", true);
    }

    ApplyMirror();
    CGangZonesSync::RefreshColours();
}
