using GTASDK;
using System;
using System.Security.Cryptography;

namespace CoopAndreasNET
{
    public class Patch
    {
        public static void PatchPlayerPed()
        {
            // Unlock pads count
            Memory.WriteByte(0x84E1FA + 1, 4 + 2); // we dont use player id 0 and 1
            Memory.WriteByte(0x856465 + 1, 4 + 2);

            // Unlock players count
            Memory.WriteByte(0x84E98A + 1, 4 + 2);
            Memory.WriteByte(0x856505 + 1, 4 + 2);

            // caused 0x706B2E crash (This seems to be ped shadow rendering)
            Memory.MakeNop(0x53EA08, 10, false);

            // Unknown from CPlayerPed::ProcessControl causes crash
            Memory.MakeNop(0x609C08, 39, false);

            // Removes the FindPlayerInfoForThisPlayerPed at these locations.
            Memory.MakeNop(0x5E63A6, 19, false);
            Memory.MakeNop(0x621AEA, 12, false);
            Memory.MakeNop(0x62D331, 11, false);
            Memory.MakeNop(0x741FFF, 27, false);

            // CPlayerPed_CPlayerPed .. task system corrupts some shit
            Memory.WriteByte(0x60D64E, 0x84); // jnz to jz

            // CPhysical Destructor (705b3b crash)
            Memory.MakeNop(0x542485, 11, false);

            // PlayerInfo checks in CPlayerPed::ProcessControl
            Memory.MakeNop(0x60F2C4, 25, false);

            // fix destroying second player and fade when distance between players too long
            Memory.MakeNop(0x442B5C, 3, false);
            Memory.MakeNop(0x859894, 6, false);

            // fix radar moving between players
            //Memory.MakeNop(0x186D23, 6, false);
            //Memory.MakeNop(0x186D29, 6, false);

            

            Console.WriteLine("patched");
        }
    }
}
