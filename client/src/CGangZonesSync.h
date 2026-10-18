#pragma once

#include "stdafx.h"
#include <CZoneInfo.h>

// gang zones (who holds which territory, the radar colours) are the same for everyone: the host runs the missions that
// change them and sends the zones that changed; the others apply them and undo their own changes (their own INITIAL
// script, scripts that don't run for them in sync)
class CGangZonesSync
{
public:
    static void InjectHooks();
    static void Reset();
    static void OnBecameHost();
    static void Process();
    static void HandlePacket(Packets::World::GangZonesUpdate* pUpdate);
    static void HandlePacket(Packets::World::GangWarsSettings* pSettings);

    // FillZonesWithGangColours, then the zone of a running war in red again, as the war itself sets it
    static void RefreshColours();

    static CZoneInfo* GetZoneInfos();
    static int GetZoneInfoCount();

private:
    static void __cdecl FillZonesWithGangColours_Hook(bool bDisableRadarGangColours);
    static bool IsAuthorityFor(int zoneIndex);
    static Packets::World::GangWarsSettings CollectSettings();
    static void ApplySettings(const Packets::World::GangWarsSettings& settings);

    static inline uint8_t ms_aBase[Packets::World::MAX_GANG_ZONES][Packets::World::GANG_COUNT]{};
    static inline bool ms_bHasBase = false;
    static inline bool ms_bSendAll = false;

    // the argument of the last FillZonesWithGangColours: whether the territories are hidden on the radar. The game
    // decides it by the call, not by a flag (SetGangWarsActive hides them, the scripts show them while wars are off)
    static inline bool ms_bRadarHidden = false;

    static inline Packets::World::GangWarsSettings ms_settings{};
    static inline bool ms_bHasSettings = false;
};
