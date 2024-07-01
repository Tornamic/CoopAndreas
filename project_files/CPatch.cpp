#include "stdafx.h"

void CPatch::ApplyPatches()
{
    CPatch::PatchPools();
    CPatch::PatchPlayers();
    CPatch::PatchStreaming();
}

void CPatch::PatchPlayers()
{
}

void CPatch::PatchStreaming()
{
    // increase available streaming memory (memory512.cs full analog) 
    patch::SetUInt(0x8A5A80, 536870912); // 512 megabytes

    // increase max fps
    RsGlobal.maxFPS = 100;
}
void CPatch::PatchPools()
{
    // you can find these addresses at 0x550F10 CPools::Initialise()
    
    // peds `140 -> 396`
    patch::SetUChar(0x550FF3, 0x01);

    // vehicles `140 -> 255`
    patch::SetUChar(0x551030, 0xFF);

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

void CPatch::FixCrashes() // thx sa-syncer
{
    //Anim crash in CPlayerPed::ProcessControl
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
}