#pragma once

// the red dot where another player aims the sniper rifle ("Sync laser sniper rifle red dot", README minor tasks): SA
// draws none for anyone (CWeapon::LaserScopeDot 0x73A8D0 is never called), the aim of the remote player comes with
// its camera sync
class CSniperDot
{
public:
    // every frame: registers the coronas for this frame
    static void Process();
};
