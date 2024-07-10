#include "stdafx.h"

void PatchPlayers()
{
    // increase player count 
    // we dont use first two players because
    // 0 - you
    // 1 - local-coop player
    patch::SetUChar(0x84E98B, MAX_SERVER_PLAYERS + 2); // _vector_constructor
    patch::SetUChar(0x856506, MAX_SERVER_PLAYERS + 2); // _vector_destructor

    // increase pad count
    // same situation
    patch::SetUChar(0x84E1FB, MAX_SERVER_PLAYERS + 2); // _vector_constructor
    patch::SetUChar(0x856466, MAX_SERVER_PLAYERS + 2); // _vector_destructor
}

#ifdef _DEV
void PatchConsole()
{
    // disable gta sa`s printfs
    
    // random floats
    patch::Nop(0x745997, 5);

    // soundmanager
    patch::Nop(0x5B97C9, 5);
}
#endif

void PatchStreaming()
{
    // increase available streaming memory (memory512.cs full analog) 
    patch::SetUInt(0x8A5A80, 536870912); // 512 megabytes
    patch::SetUInt(0x5B8E6A, 536870912); // hardcoded value

    // increase max fps
    RsGlobal.maxFPS = 100;
    patch::SetUChar(0x619626, 0x64); // hardcoded value
}

void PatchPools()
{
    // you can find these addresses at 0x550F10 CPools::Initialise()
    
    // peds `140 -> 396`
    patch::SetUChar(0x550FF3, 0x01);

    // vehicles `140 -> 255`
    // patch::SetUChar(0x551030, 0xFF);

    // tasks `500 -> 1020`
    patch::SetUChar(0x551140, 0x03);

    // events `200 -> 456`
    patch::SetUChar(0x551178, 0x01);

    // ped intelligence `150 -> 396`
    patch::SetUChar(0x551284, 0x01);

    // dummies `2500 -> 6084`
    patch::SetUChar(0x5510D0, 0x17);

    // colmodels `10150 -> 18342`
    patch::SetUChar(0x551108, 0x47);

    // objects `350 -> 862`
    patch::SetUChar(0x551098, 0x03);
    
    // matrices `900 -> 2692`
    patch::SetUChar(0x54F3A2, 0x0A);
}

void FixCrashes()
{
    // Anim crash in CPlayerPed::ProcessControl
    patch::Nop(0x609C08, 39);

    // No DirectPlay dependency
    // Increase compatibility for Windows 8+
    patch::SetUChar(0x74754A, 0xB8);
    patch::SetUInt(0x74754B, 0x900);

    // Don't create a ped group on player creation (Fixes a crash)
    patch::Nop(0x60D64D, 1);
    patch::SetUChar(0x60D64E, 0xE9);

    // Disable the call to FxSystem_c::GetCompositeMatrix in CAEFireAudioEntity::UpdateParameters
    // Which was causing a crash. The crash happens if you create 40 or
    // so vehicles that catch fire (upside down) then delete them, repeating a few times.
    patch::Nop(0x4DCF87, 6);

    // Fixed a crash (race condition triggered when jacking a vehicle)
    patch::Nop(0x6485AC, 6);

    // Fix mirror crash
    patch::SetUChar(0x7271CB + 0, 0x85); // test eax, eax
    patch::SetUChar(0x7271CB + 1, 0xC0);
    patch::SetUChar(0x7271CB + 2, 0x74); // je 0x727203
    patch::SetUChar(0x7271CB + 3, 0x34);
    patch::SetUChar(0x7271CB + 4, 0x83); // add esp, 04
    patch::SetUChar(0x7271CB + 5, 0xC4);
    patch::SetUChar(0x7271CB + 6, 0x04);
    patch::SetUChar(0x7271CB + 7, 0xC6); // mov byte ptr [00C7C728],01

    // No FxMemoryPool_c::Optimize (causes heap corruption)
    patch::Nop(0x5C25D3, 5);

    // Satchel charge crash fix
    patch::Nop(0x738F3A, 83);

    // Removes the FindPlayerInfoForThisPlayerPed at these locations.

    patch::Nop(0x5E63A6, 19);
    patch::Nop(0x621AEA, 12);
    patch::Nop(0x62D331, 11);
    patch::Nop(0x741FFF, 27);

    // fixes unknown crash from CWorld::ClearScanCodes(), test
    patch::PutRetn(0x563470);

    // CPlayerPed_CPlayerPed .. task system corrupts some shit
    patch::GetUChar(0x60D64E, 0x84); // jnz to jz

    // PlayerInfo checks in CPlayerPed::ProcessControl
    patch::Nop(0x60F2C4, 25);

    // fix crash when closing game
    patch::SetUChar(0x705B33, 0x75); // jz to jnz
}

void CPatch::ApplyPatches()
{
    PatchPools();
    PatchPlayers();
    PatchStreaming();
    FixCrashes();
#ifdef _DEV
    PatchConsole();
#endif

}