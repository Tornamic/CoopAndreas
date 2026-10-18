#include "stdafx.h"
#include "CCheatSync.h"
#include <CCheat.h>

// eCheats indices (gta-reversed source/game_sa/Enums/eCheats.h)
static constexpr uint8_t WORLD_CHEATS[] = {
    11,  // CHEAT_FASTER_CLOCK
    12,  // CHEAT_FASTER_GAMEPLAY
    13,  // CHEAT_SLOWER_GAMEPLAY
    14,  // CHEAT_PEDS_ATTACK_OTHER_WITH_GOLFCLUB
    15,  // CHEAT_HAVE_ABOUNTY_ON_YOUR_HEAD
    16,  // CHEAT_EVERYONE_ARMED
    26,  // CHEAT_BLOW_UP_ALL_CARS
    30,  // CHEAT_BGREEN_LIGHTS_CHEAT
    31,  // CHEAT_AGGRESSIVE_DRIVERS
    32,  // CHEAT_PINK_TRAFFIC
    33,  // CHEAT_BLACK_TRAFFIC
    34,  // CHEAT_CARS_ON_WATER
    35,  // CHEAT_BOATS_FLY
    39,  // CHEAT_ELVIS_IS_EVERYWHERE
    40,  // CHEAT_PEDS_ATTACK_YOU_WITH_ROCKETS
    41,  // CHEAT_BEACH_PARTY
    42,  // CHEAT_GANGMEMBERS_EVERYWHERE
    43,  // CHEAT_GANGS_CONTROLS_THE_STREETS
    44,  // CHEAT_NINJA_THEME
    46,  // CHEAT_CHEAP_TRAFFIC
    47,  // CHEAT_FAST_TRAFFIC
    48,  // CHEAT_CARS_FLY
    49,  // CHEAT_HUGE_BUNNY_HOP
    53,  // CHEAT_ALL_CARS_HAVE_NITRO
    54,  // CHEAT_CARS_FLOAT_AWAY_WHEN_HIT
    55,  // CHEAT_ALWAYS_MIDNIGHT
    56,  // CHEAT_STOP_GAME_CLOCK_ORANGE_SKY
    69,  // CHEAT_RIOT_MODE
    70,  // CHEAT_FUNHOUSE_THEME
    74,  // CHEAT_REDUCED_TRAFFIC
    75,  // CHEAT_COUNTRY_TRAFFIC
    78,  // CHEAT_ROCKET_MAYHEM
    91,  // CHEAT_ALL_TAXIS_NITRO
};

static bool IsWorldCheat(int cheatId)
{
    return std::find(std::begin(WORLD_CHEATS), std::end(WORLD_CHEATS), cheatId) != std::end(WORLD_CHEATS);
}

void __cdecl CCheatSync::OnLocalCheat(int cheatId)
{
    if (!CNetwork::m_bAuthenticated || !IsWorldCheat(cheatId))
    {
        return;
    }

    Packets::Players::PlayerCheat packet{};
    packet.cheatId = static_cast<uint8_t>(cheatId);
    packet.active = CCheat::m_aCheatsActive[cheatId];
    GetPacketFactory().Send(packet);
}

void CCheatSync::HandlePacket(Packets::Players::PlayerCheat* pPlayerCheat)
{
    int cheatId = pPlayerCheat->cheatId;
    if (!IsWorldCheat(cheatId))
    {
        return;
    }

    bool bActive = CCheat::m_aCheatsActive[cheatId];
    if (auto pFunction = CCheat::m_aCheatFunctions[cheatId])
    {
        // a function cheat either toggles its flag inside or is a one shot (never active): call it unless we are
        // already where the sender is
        if (!(pPlayerCheat->active && bActive))
        {
            pFunction();
        }
    }
    else
    {
        CCheat::m_aCheatsActive[cheatId] = pPlayerCheat->active;
    }

    if (auto pNetworkPlayer = CNetworkPlayerManager::GetPlayer(pPlayerCheat->senderPlayerid))
    {
        CChat::AddMessage("{c7a6ff}[Player] {d6dce8}%s used a cheat", pNetworkPlayer->GetName().c_str());
    }
}
