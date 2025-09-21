#include "pch.h"
#include <windows.h>

#pragma comment(linker,"/export:DllCanUnloadNow=eax_orig.DllCanUnloadNow,@1")
#pragma comment(linker,"/export:DllGetClassObject=eax_orig.DllGetClassObject,@2")
#pragma comment(linker,"/export:DllRegisterServer=eax_orig.DllRegisterServer,@3")
#pragma comment(linker,"/export:DllUnregisterServer=eax_orig.DllUnregisterServer,@4")
#pragma comment(linker,"/export:EAXDirectSoundCreate=eax_orig.EAXDirectSoundCreate,@5")
#pragma comment(linker,"/export:EAXDirectSoundCreate8=eax_orig.EAXDirectSoundCreate8,@6")
#pragma comment(linker,"/export:GetCurrentVersion=eax_orig.GetCurrentVersion,@7")

HMODULE hCoopAndreas = NULL;

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        hCoopAndreas = LoadLibraryA("CoopAndreasSA.dll");
        if (!hCoopAndreas)
        {
            MessageBoxA(
                0,
                "Failed to load CoopAndreasSA.dll.\n\n"
                "To uninstall CoopAndreas properly:\n"
                "1. Delete 'eax.dll'.\n"
                "2. Rename 'eax_orig.dll' back to 'eax.dll'.\n\n"
                "To play CoopAndreas again:\n"
                "Reinstall the mod.",
                "CoopAndreas Loader",
                MB_OK | MB_ICONERROR
            );
        }
        break;
    case DLL_PROCESS_DETACH:
        if (hCoopAndreas)
        {
            FreeLibrary(hCoopAndreas);
        }
        break;
    }
    return TRUE;
}
