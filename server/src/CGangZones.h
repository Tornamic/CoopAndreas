#pragma once

#include "network/packets/world.h"

class CNetworkPlayer;

// the gang zone table of the host, kept for the players who join later
class CGangZones
{
public:
    static void HandleUpdate(Packets::World::GangZonesUpdate* pUpdate, CNetworkPlayer* pSender);
    static void HandleSettings(Packets::World::GangWarsSettings* pSettings, CNetworkPlayer* pSender);
    static void SendState(CNetworkPlayer* pTarget);
    static void OnPlayerDisconnected(CNetworkPlayer* pNetworkPlayer);

private:
    static inline uint8_t ms_aStrength[Packets::World::MAX_GANG_ZONES][Packets::World::GANG_COUNT]{};
    static inline bool ms_abKnown[Packets::World::MAX_GANG_ZONES]{};
    static inline bool ms_bHasSettings = false;
    static inline Packets::World::GangWarsSettings ms_settings{};
};
