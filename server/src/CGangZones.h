#pragma once

#include "network/packets/world.h"

class CNetworkPlayer;

// the gang zone table of the host and the running gang war, kept for the players who join later
class CGangZones
{
public:
    static void HandleUpdate(Packets::World::GangZonesUpdate* pUpdate, CNetworkPlayer* pSender);
    static void HandleSettings(Packets::World::GangWarsSettings* pSettings, CNetworkPlayer* pSender);
    static void HandleWarState(Packets::World::GangWarState* pState, CNetworkPlayer* pSender);
    static void SendState(CNetworkPlayer* pTarget);
    static void OnPlayerDisconnected(CNetworkPlayer* pNetworkPlayer);

private:
    static inline uint8_t ms_aStrength[Packets::World::MAX_GANG_ZONES][Packets::World::GANG_COUNT]{};
    static inline bool ms_abKnown[Packets::World::MAX_GANG_ZONES]{};
    static inline bool ms_bHasSettings = false;
    static inline Packets::World::GangWarsSettings ms_settings{};

    // one war at a time: the first player whose war comes in owns it, the later ones are dropped and end theirs on
    // receiving it
    static inline CNetworkPlayer* ms_pWarOwner = nullptr;
    static inline Packets::World::GangWarState ms_warState{};
};
