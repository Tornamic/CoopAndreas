#include "stdafx.h"

// temporary disable some game content

uintptr_t CCarCtrl__GenerateRandomCars_ptr = 0x0;
uintptr_t CPlane__DoPlaneGenerationAndRemoval_ptr = 0x0;
uintptr_t CTheCarGenerators__Process_ptr = 0x0;

void CPatch::TemporaryPatches()
{
    patch::SetFloat(0x8A5B20, 0.0f); // CCarCtrl::fCarDensityMultiplier
    CTrain::DisableRandomTrains(true);
    patch::SetUChar(0x8A5B28, false); // CCarCtrl::bAllowEmergencyServicesToBeCreated
    CPlane::SwitchAmbientPlanes(false);

    // disable CCarCtrl::GenerateRandomCars
    CCarCtrl__GenerateRandomCars_ptr = injector::GetBranchDestination(0x53C1C1).as_int();
    patch::Nop(0x53C1C1, 5);

    // disable CPlane::DoPlaneGenerationAndRemoval
    CPlane__DoPlaneGenerationAndRemoval_ptr = injector::GetBranchDestination(0x434272).as_int();
    patch::Nop(0x434272, 5);

    // disable CPlane::DoPlaneGenerationAndRemoval
    CTheCarGenerators__Process_ptr = injector::GetBranchDestination(0x53C06A).as_int();
    patch::Nop(0x53C06A, 5);

    CPopulation::PedDensityMultiplier = 0.0f;
}

void CPatch::RevertTemporaryPatches()
{
    patch::SetFloat(0x8A5B20, 1.0f); // CCarCtrl::fCarDensityMultiplier
    CTrain::DisableRandomTrains(false);
    patch::SetUChar(0x8A5B28, true); // CCarCtrl::bAllowEmergencyServicesToBeCreated
    CPlane::SwitchAmbientPlanes(true);
    patch::RedirectCall(0x53C1C1, (void*)CCarCtrl__GenerateRandomCars_ptr); 
    patch::RedirectCall(0x434272, (void*)CPlane__DoPlaneGenerationAndRemoval_ptr);
    patch::RedirectCall(0x53C06A, (void*)CTheCarGenerators__Process_ptr);
    CPopulation::PedDensityMultiplier = 1.0f;
}

void CPatch::RevertTemporaryPatchesForHost()
{
    patch::SetFloat(0x8A5B20, 1.0f); // CCarCtrl::fCarDensityMultiplier
    CTrain::DisableRandomTrains(false);
    patch::SetUChar(0x8A5B28, true); // CCarCtrl::bAllowEmergencyServicesToBeCreated
    CPlane::SwitchAmbientPlanes(true);
    patch::RedirectCall(0x434272, (void*)CPlane__DoPlaneGenerationAndRemoval_ptr);
    CPopulation::PedDensityMultiplier = 1.0f;
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

void __declspec(naked) RsMouseSetPos_Reimpl(RwV2d* pos)
{
    if (GetForegroundWindow() == RsGlobal.ps->window)
    {
        __asm
        {
            push 0x7453F0
            ret
        }
    }
    __asm
    {
        ret
    }
}

void PatchStreaming()
{
    // increase available streaming memory (memory512.cs full analog) 
    patch::SetUInt(0x8A5A80, 536870912); // 512 megabytes
    patch::SetUInt(0x5B8E6A, 536870912); // hardcoded value

    // patch game freezeing if inactive
    patch::Nop(0x561AF0, 7); // dont pause the game loop if paused
    patch::Nop(0x745BC9, 2); // unlock resolutions
    //patch::SetUChar(0x747FB6, 1);
    patch::SetUChar(0x74805A, 1); // ForegroundApp always 1
    patch::Nop(0x53EA88, 6);

    // do not hide the cursor on the control box of the game window
    patch::Nop(0x747FE9, 8);

    // PROPER MOUSE FIX
    patch::SetUChar(0x576CCC, 0xEB);
    patch::SetUChar(0x576EBA, 0xEB);
    patch::SetUChar(0x576F8A, 0xEB);
    patch::SetUInt(0x7469A0, 0x9090C030);
    patch::RedirectJump(0x6194A0, RsMouseSetPos_Reimpl); // dont set cursor pos to center if im not focused
}


void PatchPools()
{
    // WARNING
    // if we add replay support, the limits of vehicles and peds should be `< 256`
    // otherwise need to patch the major part of replays

    // ped pool (255)
    // push    8Ch -> push    FFh
    patch::SetRaw(0x550FF1, (void*)"\x68\xFF\x00\x00\x00", 5); 

    // intelligence pool (255), must be equal to the size of the ped pool
    // push    8Ch -> push    FFh
    patch::SetRaw(0x551282, (void*)"\x68\xFF\x00\x00\x00", 5); 
    
    // vehicle pool (400)
    // push    offset aVehicles -> push    0h
    // push    6Eh              -> push    190h
    patch::SetRaw(0x551024, (void*)"\x6A\x00\x68\x90\x01\x00\x00", 7); 

    // EntryInfoNode pool (1012)
    // push    1F4h -> push    3F4h
    patch::SetUChar(0x550FB9 + 0x2, 0x3);
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
    /*patch::Nop(0x60D64D, 1);
    patch::SetUChar(0x60D64E, 0xE9);*/

    // Disable the call to FxSystem_c::GetCompositeMatrix in CAEFireAudioEntity::UpdateParameters
    // Which was causing a crash. The crash happens if you create 40 or
    // so vehicles that catch fire (upside down) then delete them, repeating a few times.
    //patch::Nop(0x4DCF87, 6);

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
    //patch::Nop(0x4407B7, 5);
    //patch::Nop(0x61648C, 5);

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

    // disable stunt jump game speed changing
    patch::Nop(0x49C892, 10);

    // fix CBike controlling, wtf R* did
    patch::Nop(0x6BC9EB, 2);

    DWORD temp;

    // unprotect memory for the camera context switching patches, see CAimSync::ApplyNetworkPlayerContext and CAimSync::ApplyLocalContext
    injector::UnprotectMemory(0x50AB10, 1, temp);
    injector::UnprotectMemory(0x50BFB0, 3, temp);
    injector::UnprotectMemory(0x50BFF0, 3, temp);
    injector::UnprotectMemory(0x609C80, 1, temp);

    injector::UnprotectMemory(0x6884C4, 6, temp); // see PlayerHooks.cpp

    // temporary solution to fix jerking of dead vehicles
    patch::SetUChar(0x6C25DB, 0xEB);

    patch::Nop(0x5B1930, 11);
    patch::Nop(0x5B1942, 5);

    // looks like a solution to fix duplication of peds when creating at attractors (please)
    patch::SetFloat(0x86D26C, 10.0f);

    // patch isAlreadyRunning, so we can run more than one game instance
    if (GetModuleHandleA("SilentPatchSA.asi"))
    {
        patch::SetRaw(0x74872D, (void*)"\xE8\xAE\xE1\xFF\xFF\x85\xC0\x75\x0F", 9);
        patch::PutRetn0(0x7468E0);
    }
    else
    {
        patch::SetUChar(0x406946, 0xDC);
        patch::PutRetn0(0x7468E0);
        patch::Nop(0x74872D, 9); 
    }

    // allow vertical camera movement during a camera fade
    patch::SetUChar(0x524084, 0xFF);
    patch::SetUChar(0x524089, 0xFF);
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

bool IsFileExistsInModuleDir(const char* relativePath)
{
    char exePath[MAX_PATH];
    if (!GetModuleFileNameA(NULL, exePath, MAX_PATH))
        return false;

    char* lastSlash = strrchr(exePath, '\\');
    if (!lastSlash)
        return false;
    *lastSlash = '\0';

    char fullPath[MAX_PATH];
    snprintf(fullPath, MAX_PATH, "%s\\%s", exePath, relativePath);

    DWORD attrs = GetFileAttributesA(fullPath);
    return (attrs != INVALID_FILE_ATTRIBUTES) && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
}

const char aScriptDir[] = "CoopAndreas";
const char aScriptPath[] = "CoopAndreas\\main.scm";
const char aImgPath[] = "CoopAndreas\\script.img";

uintptr_t CStreaming__AddImageToList_ptr = 0x0;
uint32_t CStreaming__AddImageToList_Hook(const char* fileName, bool notPlayerFile)
{
    if (strncmp(fileName, "DATA\\SCRIPT\\SCRIPT.IMG", 22) == 0)
    {
        fileName = aImgPath;

        if (!IsFileExistsInModuleDir(aImgPath))
        {
            MessageBoxA(NULL, "'CoopAndreas\\script.img' is not found, try to reinstall the mod", "CoopAndreas Fatal Error", MB_ICONERROR);
            exit(0);
        }
    }

    return plugin::CallAndReturnDyn<uint32_t>(CStreaming__AddImageToList_ptr, fileName, notPlayerFile);
}

void PatchSCM()
{
    // fix modloader compatibility
    patch::RedirectCall(0x53BC95, (void*)0x5B9030);
    patch::RedirectCall(0x53BC9B, (void*)0x5B9030);
    patch::RedirectCall(0x748CFB, (void*)0x53E580);

    if (!IsFileExistsInModuleDir(aScriptPath))
    {
        MessageBoxA(NULL, "'CoopAndreas\\main.scm' is not found, try to reinstall the mod", "CoopAndreas Fatal Error", MB_ICONERROR);
        exit(0);
    }

    // use our main.scm
    patch::SetPointer(0x468EB5 + 1, (void*)aScriptDir);
    patch::SetPointer(0x489A45 + 1, (void*)aScriptPath);


    // hook script.img loading
    CStreaming__AddImageToList_ptr = injector::GetBranchDestination(0x5B915B).as_int();
    patch::RedirectCall(0x5B915B, CStreaming__AddImageToList_Hook);
}

void CPatch::ApplyPatches()
{
    PatchSCM();
    PatchPools();
    PatchStreaming();
    FixCrashes();
#ifdef _DEV
    PatchConsole();
#endif
    RelocateScanCodes();
    PatchLoadScreen();
}
