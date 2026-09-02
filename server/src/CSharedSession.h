#pragma once

class CNetworkPlayer;

namespace Packets::Players
{
class PlayerStats;
}

class CSharedSession
{
public:
    static void ApplyFromPlayer(CNetworkPlayer* player, Packets::Players::PlayerStats* packet);
    static void SendToPlayer(CNetworkPlayer* target);
    static bool IsActive();
};
