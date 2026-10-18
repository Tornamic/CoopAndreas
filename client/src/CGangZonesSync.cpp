#include "stdafx.h"
#include "CGangZonesSync.h"
#include "CGangWarSync.h"
#include <CTheZones.h>
#include <CGangWars.h>

using namespace Packets::World;

// plugin-sdk names CTheZones::ZoneInfoArray the navigation zones (0xBA3798), the zone infos are here
CZoneInfo* CGangZonesSync::GetZoneInfos()
{
    return reinterpret_cast<CZoneInfo*>(0xBA1DF0);
}

int CGangZonesSync::GetZoneInfoCount()
{
    return std::clamp<int>(*reinterpret_cast<int16_t*>(0xBA1DE8), 0, MAX_GANG_ZONES);
}

// CTheZones::NavigationZoneArray: a CZone is 0x20 bytes in the game, plugin-sdk's is 0x30 (a whole CZoneInfo in place of
// the index), so no pointer arithmetic on CZone*
static constexpr uintptr_t NAVIGATION_ZONES = 0xBA3798;
static constexpr int ZONE_SIZE = 0x20;

CZone* CGangZonesSync::GetNavigationZone(int index)
{
    int nCount = *reinterpret_cast<int16_t*>(0xBA3794);
    return index >= 0 && index < nCount ? reinterpret_cast<CZone*>(NAVIGATION_ZONES + index * ZONE_SIZE) : nullptr;
}

int CGangZonesSync::GetNavigationZoneIndex(const CZone* pZone)
{
    return pZone != nullptr ? static_cast<int>((reinterpret_cast<uintptr_t>(pZone) - NAVIGATION_ZONES) / ZONE_SIZE) : -1;
}

CZoneInfo* CGangZonesSync::GetZoneInfo(const CZone* pZone)
{
    // CZone::m_ZoneInfoIndex, 0 is no info
    int16_t nIndex = *reinterpret_cast<const int16_t*>(reinterpret_cast<const uint8_t*>(pZone) + 0x1C);
    return nIndex > 0 && nIndex < GetZoneInfoCount() ? GetZoneInfos() + nIndex : nullptr;
}

void __cdecl CGangZonesSync::FillZonesWithGangColours_Hook(bool bDisableRadarGangColours)
{
    ms_bRadarHidden = bDisableRadarGangColours;
    plugin::Call<0x572440, bool>(bDisableRadarGangColours);
}

void CGangZonesSync::InjectHooks()
{
    // CTheZones::FillZonesWithGangColours callers; ClearSpecificZonesToTriggerGangWar jumps to it
    for (uintptr_t address : {0x446483, 0x446548, 0x446584, 0x446A3D, 0x446B8D, 0x446C43, 0x446D44, 0x4685E9, 0x618F3B,
             0x156DD4C})
    {
        patch::RedirectCall(address, FillZonesWithGangColours_Hook);
    }
    patch::RedirectJump(0x44402E, FillZonesWithGangColours_Hook);
}

void CGangZonesSync::Reset()
{
    ms_bHasBase = false;
    ms_bSendAll = false;
    ms_bHasSettings = false;
}

void CGangZonesSync::OnBecameHost()
{
    // the server may have nothing yet or a table of the previous host that we have anyway
    ms_bSendAll = true;
    ms_bHasSettings = false;
}

bool CGangZonesSync::IsAuthorityFor(int zoneIndex)
{
    return CLocalPlayer::m_bIsHost || zoneIndex == CGangWarSync::GetOwnedZoneInfo();
}

GangWarsSettings CGangZonesSync::CollectSettings()
{
    GangWarsSettings settings{};
    settings.active = CGangWars::bGangWarsActive;
    settings.radarHidden = ms_bRadarHidden;
    settings.trainingMission = CGangWars::bTrainingMission;
    settings.trainingZone = static_cast<int16_t>(std::clamp(CGangWars::ZoneInfoForTraining, -1, MAX_GANG_ZONES - 1));
    settings.canTriggerOnMission = CGangWars::bCanTriggerGangWarWhenOnAMission;
    settings.specificZoneCount =
        static_cast<uint8_t>(std::clamp(CGangWars::NumSpecificZones, 0, MAX_GANG_WAR_SPECIFIC_ZONES));
    for (int i = 0; i < settings.specificZoneCount; i++)
    {
        settings.specificZones[i] = static_cast<int16_t>(std::clamp(CGangWars::aSpecificZones[i], 0, MAX_GANG_ZONES - 1));
    }
    return settings;
}

void CGangZonesSync::ApplySettings(const GangWarsSettings& settings)
{
    // the fields as they are: SetGangWarsActive would also end a war and reset the attack timer
    CGangWars::bGangWarsActive = settings.active;
    ms_bRadarHidden = settings.radarHidden;
    CGangWars::bTrainingMission = settings.trainingMission;
    CGangWars::ZoneInfoForTraining = settings.trainingZone;
    CGangWars::bCanTriggerGangWarWhenOnAMission = settings.canTriggerOnMission;
    CGangWars::NumSpecificZones = settings.specificZoneCount;
    for (int i = 0; i < settings.specificZoneCount; i++)
    {
        CGangWars::aSpecificZones[i] = settings.specificZones[i];
    }
}

void CGangZonesSync::RefreshColours()
{
    FillZonesWithGangColours_Hook(ms_bRadarHidden);

    CZoneInfo* pWarZone = CGangWars::pZoneInfoToFightOver;
    if ((CGangWars::State != NOT_IN_WAR || CGangWars::State2 != NO_ATTACK) && pWarZone != nullptr)
    {
        // RadarMode (bits 5-6 of the flags) = 2, the colour StartOffensiveGangWar gives the zone
        pWarZone->m_nFlags = static_cast<char>((pWarZone->m_nFlags & ~0x60) | (2 << 5));
        pWarZone->m_ZoneColor = CRGBA(255, 0, 0, 160);
    }
}

void CGangZonesSync::Process()
{
    CZoneInfo* pZoneInfos = GetZoneInfos();
    int nZoneCount = GetZoneInfoCount();
    bool bRecolour = false;

    GangZonesUpdate update{};
    for (int i = 0; i < nZoneCount; i++)
    {
        uint8_t* pStrength = reinterpret_cast<uint8_t*>(pZoneInfos[i].m_nGangDensity);
        bool bChanged = memcmp(pStrength, ms_aBase[i], GANG_COUNT) != 0;

        if (IsAuthorityFor(i))
        {
            if (bChanged || ms_bSendAll || !ms_bHasBase)
            {
                auto& zone = update.zones[update.count++];
                zone.index = static_cast<uint16_t>(i);
                memcpy(zone.strength, pStrength, GANG_COUNT);
                memcpy(ms_aBase[i], pStrength, GANG_COUNT);
            }
        }
        else if (bChanged && ms_bHasBase)
        {
            memcpy(pStrength, ms_aBase[i], GANG_COUNT);
            bRecolour = true;
        }
    }

    if (update.count > 0)
    {
        GetPacketFactory().Send(update);

        // the others recolour on receiving, the game would do it only on its next FillZonesWithGangColours
        bRecolour = true;
    }

    if (CLocalPlayer::m_bIsHost)
    {
        ms_bHasBase = true;
        ms_bSendAll = false;

        GangWarsSettings settings = CollectSettings();
        if (!ms_bHasSettings || !(settings == ms_settings))
        {
            GetPacketFactory().Send(settings);
            ms_settings = settings;
            ms_bHasSettings = true;
        }
    }
    else if (ms_bHasSettings && !(CollectSettings() == ms_settings))
    {
        ApplySettings(ms_settings);
        bRecolour = true;
    }

    if (bRecolour)
    {
        RefreshColours();
    }
}

void CGangZonesSync::HandlePacket(GangZonesUpdate* pUpdate)
{
    CZoneInfo* pZoneInfos = GetZoneInfos();
    int nZoneCount = GetZoneInfoCount();

    for (int i = 0; i < pUpdate->count; i++)
    {
        const auto& zone = pUpdate->zones[i];
        memcpy(ms_aBase[zone.index], zone.strength, GANG_COUNT);
        if (zone.index < nZoneCount)
        {
            memcpy(pZoneInfos[zone.index].m_nGangDensity, zone.strength, GANG_COUNT);
        }
    }

    // a full table comes first from the server, a part of it could only be a change of the host
    ms_bHasBase = true;
    RefreshColours();
}

void CGangZonesSync::HandlePacket(GangWarsSettings* pSettings)
{
    ms_settings = *pSettings;
    ms_bHasSettings = true;

    if (!CLocalPlayer::m_bIsHost)
    {
        ApplySettings(ms_settings);
        RefreshColours();
    }
}
