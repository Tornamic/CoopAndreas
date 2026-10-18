#pragma once

// cheats that change the world (traffic, peds, time, physics) apply to every player: a world for everyone. Personal
// ones (health, weapons, vehicles, skills, wanted level) stay with whoever typed them; weather goes by CWeatherSync
class CCheatSync
{
public:
    // CCheat::AddToCheatString has just applied a typed cheat
    static void __cdecl OnLocalCheat(int cheatId);
    static void HandlePacket(Packets::Players::PlayerCheat* pPlayerCheat);
};
