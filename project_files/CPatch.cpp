#include "stdafx.h"

// temporary disable some game content
void CPatch::TemporaryPatches()
{
    // implementation of my patch.lua script
    patch::SetFloat(0x8A5B20, 0.0f); // disable traffic
    CTrain::DisableRandomTrains(0); // disable trains
    patch::SetUChar(0x8A5B28, false); // disable EMS
    //CPlane::SwitchAmbientPlanes(false); // disable ambient planes
    //patch::SetUInt(0x9690A0, 0); // disable car generator CCarCtrl::NumParkedCars (count, not func)
    //patch::Nop(0x53C1C1, 5); // disable CCarCtrl::GenerateRandomCars
    //patch::Nop(0x434272, 5); // disable CPlane::DoPlaneGenerationAndRemoval
    patch::SetUShort(0x8D477C, 0); // disable spawn 537 train
    CPopulation::PedDensityMultiplier = 0.0f;
}

void CPatch::PatchFramerate()
{
    unsigned int fps = 60;

    HWND hwnd = GetForegroundWindow();

    // get current display from window
    HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

    if (hMonitor) 
    {
        MONITORINFOEX mi;
        mi.cbSize = sizeof(MONITORINFOEX);

        // get display info
        if (GetMonitorInfo(hMonitor, &mi)) 
        {
            DEVMODE devMode;
            devMode.dmSize = sizeof(DEVMODE);

            if (EnumDisplaySettings(mi.szDevice, ENUM_CURRENT_SETTINGS, &devMode)) 
            {
                // get refresh rate (HZ)
                fps = devMode.dmDisplayFrequency;
            }
        }
    }

    patch::SetUChar(0x53E94C, 0); // remove 14 ms frame delay
    patch::SetUChar(0x619626, fps); // init value
    patch::SetUChar(0xC1704C, fps, false); // current value
    RwD3D9EngineSetRefreshRate(fps); // update refresh rate
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

    // patch game freezeing if inactive
    patch::Nop(0x561AF0, 7);
    patch::Nop(0x745BC9, 2);
    patch::SetUChar(0x747FB6, 1);
    patch::SetUChar(0x74805A, 1);
    patch::Nop(0x74542B, 8);
    patch::Nop(0x53EA88, 6);
}


void PatchPools()
{
    // TODO: increase ped, intelligence, vehicle limits to 255
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

    // remove CPlayerData -> CanDoDriveBy checks
    patch::Nop(0x5E63A6, 19);
    patch::Nop(0x621AEA, 12);
    patch::Nop(0x62D331, 11);
    patch::Nop(0x741FFF, 27);
    patch::Nop(0x60F2C4, 25); //CPlayerPed::ProcessControl

    // nop ped destroying when player enters interior (remove after separating ped sync)
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

    // fix driver kill bug sprint pressed (possible issues with missions and scripts)
    patch::SetUChar(0x62F223, 0);
}

#define SCANCODE_BUFFER_SIZE (8 * 20000)

unsigned char ScanCodeBuffer[SCANCODE_BUFFER_SIZE];

const unsigned int RelocateScanCodesPatch1[13] = {
    0x5DC7AA, 0x41A85D, 0x41A864, 0x408259, 0x711B32, 0x699CF8,
    0x4092EC, 0x408702, 0x564220, 0x564172, 0x563845, 0x84E9C2, 
    0x85652D
};

const unsigned int RelocateScanCodesPatch2[56] = {
    0x0040D68C, 0x005664D7, 0x00566586, 0x00408706, 0x0056B3B1, 0x0056AD91,
    0x0056A85F, 0x005675FA, 0x0056CD84, 0x0056CC79, 0x0056CB51, 0x0056CA4A,
    0x0056C664, 0x0056C569, 0x0056C445, 0x0056C341, 0x0056BD46, 0x0056BC53,
    0x0056BE56, 0x0056A940, 0x00567735, 0x00546738, 0x0054BB23, 0x006E31AA,
    0x0040DC29, 0x00534A09, 0x00534D6B, 0x00564B59, 0x00564DA9, 0x0067FF5D,
    0x00568CB9, 0x00568EFB, 0x00569F57, 0x00569537, 0x00569127, 0x0056B4B5,
    0x0056B594, 0x0056B2C3, 0x0056AF74, 0x0056AE95, 0x0056BF4F, 0x0056ACA3,
    0x0056A766, 0x0056A685, 0x0070B9BA, 0x0056479D, 0x0070ACB2, 0x006063C7,
    0x00699CFE, 0x0041A861, 0x0040E061, 0x0040DF5E, 0x0040DDCE, 0x0040DB0E,
    0x0040D98C, 0x01566855
};

const unsigned int RelocateScanCodesPatch3[12] = {
    0x004091C5, 0x00409367, 0x0040D9C5, 0x0040DB47, 0x0040DC61, 0x0040DE07,
    0x0040DF97, 0x0040E09A, 0x00534A98, 0x00534DFA, 0x0071CDB0, 0x40914E
};

const unsigned int RelocateScanCodesPatch4[4] = { 0x005634A6, 0x005638DF, 0x0056420F, 0x00564283 };

void RelocateScanCodes()
{
    DWORD oldProt;
    memset(ScanCodeBuffer, 0, SCANCODE_BUFFER_SIZE);
    unsigned char* scanCodeBufferBase = &ScanCodeBuffer[0];

    for (int i = 0; i < 13; i++) {
        VirtualProtect((PVOID)RelocateScanCodesPatch1[i], 4, PAGE_EXECUTE_READWRITE, &oldProt);
        *(PDWORD)RelocateScanCodesPatch1[i] = (DWORD)scanCodeBufferBase;
        VirtualProtect((PVOID)RelocateScanCodesPatch1[i], 4, oldProt, &oldProt);
    }

    for (int i = 0; i < 56; i++) {
        VirtualProtect((PVOID)RelocateScanCodesPatch2[i], 8, PAGE_EXECUTE_READWRITE, &oldProt);
        *(PDWORD)(RelocateScanCodesPatch2[i] + 3) = (DWORD)scanCodeBufferBase;
        VirtualProtect((PVOID)RelocateScanCodesPatch2[i], 8, oldProt, &oldProt);
    }

    for (int i = 0; i < 12; i++) {
        VirtualProtect((PVOID)RelocateScanCodesPatch3[i], 8, PAGE_EXECUTE_READWRITE, &oldProt);
        *(PDWORD)(RelocateScanCodesPatch3[i] + 3) = (DWORD)(scanCodeBufferBase + 4);
        VirtualProtect((PVOID)RelocateScanCodesPatch3[i], 8, oldProt, &oldProt);
    }
    
    for (int i = 0; i < 4; i++) {
        VirtualProtect((PVOID)RelocateScanCodesPatch4[i], 4, PAGE_EXECUTE_READWRITE, &oldProt);
        *(PDWORD)RelocateScanCodesPatch4[i] = (DWORD)(scanCodeBufferBase + sizeof(ScanCodeBuffer));
        VirtualProtect((PVOID)RelocateScanCodesPatch4[i], 4, oldProt, &oldProt);
    }

    VirtualProtect((PVOID)0x40936A, 4, PAGE_EXECUTE_READWRITE, &oldProt);
    *(PDWORD)0x40936A = (DWORD)(scanCodeBufferBase + 4);
    VirtualProtect((PVOID)0x40936A, 4, oldProt, &oldProt);

    VirtualProtect((PVOID)0x564DC7, 4, PAGE_EXECUTE_READWRITE, &oldProt);
    *(PDWORD)0x564DC7 = (DWORD)(scanCodeBufferBase + 115200);
    VirtualProtect((PVOID)0x564DC7, 4, oldProt, &oldProt);

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
    //PatchPools();
    PatchStreaming();
    FixCrashes();
#ifdef _DEV
    PatchConsole();
#endif
    RelocateScanCodes();
    PatchLoadScreen();
}
