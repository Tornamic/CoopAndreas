#include "stdafx.h"

void PatchPlayers()
{
    // i think this patches are useless
    
    // increase player count 
    // we dont use first two players because
    // 0 - you
    // 1 - local-coop player
    //patch::SetUChar(0x84E98B, MAX_SERVER_PLAYERS + 2); // _vector_constructor
    //patch::SetUChar(0x856506, MAX_SERVER_PLAYERS + 2); // _vector_destructor

    // increase pad count
    // same situation
    //patch::SetUChar(0x84E1FB, MAX_SERVER_PLAYERS + 2); // _vector_constructor
    //patch::SetUChar(0x856466, MAX_SERVER_PLAYERS + 2); // _vector_destructor
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
    //RsGlobal.maxFPS = 100;
    //patch::SetUChar(0x619626, 0x64); // hardcoded value

    // patch game freezeing if inactive
    patch::Nop(0x561AF0, 7);
    patch::Nop(0x745BC9, 2);
    patch::SetUChar(0x747FB6, 1);
    patch::SetUChar(0x74805A, 1);
    patch::Nop(0x74542B, 8);
    patch::Nop(0x53EA88, 6);

    // fix cutscene crash
    patch::Nop(0x40EC56, 5);

    // fix crash after death
    //patch::Nop(0x441481, 6);
}


void PatchPools()
{
    //Inc task pool
    patch::SetUChar(0x551140, 0xFF);

    //Inc ped pool pool
    patch::SetUChar(0x550FF2, (unsigned char)1000);

    //Inc intelligence pool
    patch::SetUChar(0x551283, 210);

    //Inc event pool
    patch::SetUChar(0x551178, 0x01);

    //Inc matrices pool
    patch::SetUChar(0x54F3A2, 0x10);

    //Inc ccolmodel pool
    patch::SetUChar(0x551108, 0x77);

    //Inc dummies pool
    patch::SetUChar(0x5510D0, 0x0F);

    //Inc objects pool
    patch::SetUChar(0x551098, 0x02);
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

    // PlayerInfo checks in CPlayerPed::ProcessControl
    patch::Nop(0x60F2C4, 25);

    // nop ped destroying when player enters interior
    patch::Nop(0x4407B7, 5);
    patch::Nop(0x61648C, 5);

    // fix spawning with bottles and cigars
    patch::Nop(0x4217F4, 21);
    patch::Nop(0x4218D8, 17);
    patch::Nop(0x5F80C0, 10);
    patch::Nop(0x5FBA47, 10);

    // disable vehicle audio (controls manualy from hooks)
    patch::Nop(0x6B18F1, 5);
    patch::Nop(0x6B9298, 5);
    patch::Nop(0x6F1793, 5);
    patch::Nop(0x6F86B6, 5);

    // fixes CPhysical dtor crash 
    // (0x00705B3B _ZN22CRealTimeShadowManager20ReturnRealTimeShadowEP15CRealTimeShadow)
    // Access violation writing location 0x00000134
    // hook and check `ecx` for 0x0?.. nah, we can just NOP it!
    patch::Nop(0x542485, 11);

    // another one shadow crash... NOP IT AGAIN
    patch::Nop(0x53EA08, 10);

    // disable replays
    patch::Nop(0x53C090, 5);
}

#define SCANCODE_SIZE 8*20000

unsigned char ScanCodeMemory[SCANCODE_SIZE];

const unsigned int relocateScanCodesPatch1[14] = {
0x5DC7AA,0x41A85D,0x41A864,0x408259,0x711B32,0x699CF8,
0x4092EC,0x40914E,0x408702,0x564220,0x564172,0x563845,
0x84E9C2,0x85652D };

const unsigned int relocateScanCodesPatch2[56] = {
0x0040D68C,0x005664D7,0x00566586,0x00408706,0x0056B3B1,0x0056AD91,0x0056A85F,0x005675FA,
0x0056CD84,0x0056CC79,0x0056CB51,0x0056CA4A,0x0056C664,0x0056C569,0x0056C445,0x0056C341,
0x0056BD46,0x0056BC53,0x0056BE56,0x0056A940,0x00567735,0x00546738,0x0054BB23,0x006E31AA,
0x0040DC29,0x00534A09,0x00534D6B,0x00564B59,0x00564DA9,0x0067FF5D,0x00568CB9,0x00568EFB,
0x00569F57,0x00569537,0x00569127,0x0056B4B5,0x0056B594,0x0056B2C3,0x0056AF74,0x0056AE95,
0x0056BF4F,0x0056ACA3,0x0056A766,0x0056A685,0x0070B9BA,0x0056479D,0x0070ACB2,0x006063C7,
0x00699CFE,0x0041A861,0x0040E061,0x0040DF5E,0x0040DDCE,0x0040DB0E,0x0040D98C,0x01566855 };

unsigned int relocateScanCodesPatch3[11] = {
0x004091C5,0x00409367,0x0040D9C5,0x0040DB47,0x0040DC61,0x0040DE07,0x0040DF97,
0x0040E09A,0x00534A98,0x00534DFA,0x0071CDB0 };

unsigned int relocateScanCodesPatch4[4] = { 0x005634A6, 0x005638DF, 0x0056420F, 0x00564283 };


//-----------------------------------------------------------

void RelocateScanCodes()
{
    DWORD oldProt;
    memset(&ScanCodeMemory[0], 0, SCANCODE_SIZE);
    unsigned char* aScanCodeMemory = &ScanCodeMemory[0];

    int x = 0;
    while (x != 14) 
    {
        VirtualProtect((PVOID)relocateScanCodesPatch1[x], 4, PAGE_EXECUTE_READWRITE, &oldProt);
        *(PDWORD)relocateScanCodesPatch1[x] = (DWORD)aScanCodeMemory;
        x++;
    }

    x = 0;
    while (x != 56) 
    {
        VirtualProtect((PVOID)relocateScanCodesPatch2[x], 8, PAGE_EXECUTE_READWRITE, &oldProt);
        *(PDWORD)(relocateScanCodesPatch2[x] + 3) = (DWORD)aScanCodeMemory;
        x++;
    }

    // THIRD LIST THAT POINTS TO THE BASE SCANLIST MEMORY + 4
    x = 0;
    while (x != 11) {
        VirtualProtect((PVOID)relocateScanCodesPatch3[x], 8, PAGE_EXECUTE_READWRITE, &oldProt);
        *(PDWORD)(relocateScanCodesPatch3[x] + 3) = (DWORD)(aScanCodeMemory + 4);
        x++;
    }

    // FOURTH LIST THAT POINTS TO THE END OF THE SCANLIST
    x = 0;
    while (x != 4) {
        VirtualProtect((PVOID)relocateScanCodesPatch4[x], 4, PAGE_EXECUTE_READWRITE, &oldProt);
        *(PDWORD)(relocateScanCodesPatch4[x]) = (DWORD)(aScanCodeMemory + sizeof(aScanCodeMemory));
        x++;
    }

    // Others that didn't fit.
    VirtualProtect((PVOID)0x40936A, 4, PAGE_EXECUTE_READWRITE, &oldProt);
    *(PDWORD)0x40936A = (DWORD)(aScanCodeMemory + 4);

    // Reset the exe scanlist mem for playerinfo's
    memset((BYTE*)0xB7D0B8, 0, 8 * 14400);
}

void SimulateCopyrightScreen()
{
    CLoadingScreen::m_currDisplayedSplash = 0;
    CLoadingScreen::m_timeSinceLastScreen -= 1000.f;
    CLoadingScreen::m_bFadeInNextSplashFromBlack = 1;
}

void PatchLoadScreen()
{
    // Start the game at state 5
    // Disable gGameState = 0 setting
    patch::Nop(0x747483, 6);

    // Put the game where the user wants (default's to the copyright screen)
    // GS_INIT_ONCE:5
    patch::Set(0xC8D4C0, 5);

    // Disable Copyright screen
    // Hook the copyright screen fading in/out and simulates that it has happened
    patch::Nop(0x748C2B, 5);
    patch::RedirectCall(0x748C9A, SimulateCopyrightScreen);
}

void CPatch::ApplyPatches()
{
    // this comment fixes a lot of crashes :D
    //PatchPools(); 
    
    PatchPlayers();
    PatchStreaming();
    FixCrashes();
#ifdef _DEV
    PatchConsole();
#endif
    RelocateScanCodes();
    PatchLoadScreen();
}
