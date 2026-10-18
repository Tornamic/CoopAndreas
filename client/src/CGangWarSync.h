#pragma once

#include "stdafx.h"

// a gang war runs in CGangWars::Update of one player, its owner: the attack waves are its mission peds (network peds
// for the others), it decides when a wave is beaten and changes the zone. While it runs the others skip their own
// CGangWars::Update and mirror it: the red zone, no civilians, the texts, blips on the attackers
class CGangWarSync
{
public:
    static void InjectHooks();
    static void Reset();
    // before CGangZonesSync::Process: the owner changes the zone of its war, the change goes before the war state
    static void PreProcess();
    static void Process();
    static void HandlePacket(Packets::World::GangWarState* pState);

    static bool IsMirroring() { return ms_bMirroring; }
    // the zone info index the local war is fought for, -1 without one
    static int GetOwnedZoneInfo() { return ms_nOwnedZoneInfo; }

private:
    struct SWaveBlip
    {
        pedid_t pedid{};
        CPed* pPed = nullptr;
        int nHandle = -1;
    };

    static bool IsLocalWarRunning();
    static Packets::World::GangWarState Collect();
    static uint8_t GetEndResult();
    static void KeepOwnerInArea();
    static void ApplyMirror();
    static void EndMirror(uint8_t result);
    static void UpdateWaveBlips();
    static void ClearWaveBlips();
    static void UpdateAttackBlip();
    static unsigned int GetGangColour(int gang);
    static bool IsNearWarZone(const CVector& position);
    static void ShowText(const char* gxt, bool bHelp = false);

    static void __cdecl CGangWars__Update_Hook();
    static void __cdecl CGangWars__StartDefensiveGangWar_Hook();
    static void __cdecl CGangWars__DoStuffWhenPlayerVictorious_Hook();

    // owner
    static inline bool ms_bWasRunning = false;
    static inline bool ms_bVictory = false;
    static inline int ms_nOwnedZoneInfo = -1;
    static inline Packets::World::GangWarState ms_lastSent{};

    // the others
    static inline bool ms_bMirroring = false;
    static inline Packets::World::GangWarState ms_mirror{};
    static inline std::vector<SWaveBlip> ms_waveBlips;
    static inline int ms_nAttackBlip = -1;
};
