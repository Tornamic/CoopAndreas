#include "pch.h"
#include <cstdio>
#include <cstring>
#include <errhandlingapi.h>
#include <windows.h>

#pragma comment(linker, "/export:DllCanUnloadNow=eax_orig.DllCanUnloadNow,@1")
#pragma comment(linker, "/export:DllGetClassObject=eax_orig.DllGetClassObject,@2")
#pragma comment(linker, "/export:DllRegisterServer=eax_orig.DllRegisterServer,@3")
#pragma comment(linker, "/export:DllUnregisterServer=eax_orig.DllUnregisterServer,@4")
#pragma comment(linker, "/export:EAXDirectSoundCreate=eax_orig.EAXDirectSoundCreate,@5")
#pragma comment(linker, "/export:EAXDirectSoundCreate8=eax_orig.EAXDirectSoundCreate8,@6")
#pragma comment(linker, "/export:GetCurrentVersion=eax_orig.GetCurrentVersion,@7")

bool NeedToInjectCoopAndreas()
{
    LPTSTR cmd = GetCommandLine();
    return strstr(cmd, "--coop") != nullptr;
}

HMODULE hCoopAndreas = NULL;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            if (!NeedToInjectCoopAndreas())
            {
                break;
            }
            hCoopAndreas = LoadLibrary("CoopAndreasSA.dll");
            if (!hCoopAndreas)
            {
                DWORD dwError = GetLastError();
                char msg[512];
                switch (dwError)
                {
                    case ERROR_MOD_NOT_FOUND:
                        sprintf(msg,
                            "Failed to load CoopAndreasSA.dll.\n\n"
                            "GetLastError() == %u\n\n"
                            "Some of the dependencies are missing.\n\n"
                            "Installing DirectX End-User Runtimes (June 2010) may fix this error.\n"
                            "https://www.microsoft.com/en-us/download/details.aspx?id=8109",
                            dwError);
                        break;
                    default:
                        sprintf(msg,
                            "Failed to load CoopAndreasSA.dll.\n\n"
                            "To uninstall CoopAndreas properly:\n"
                            "1. Delete 'eax.dll'.\n"
                            "2. Rename 'eax_orig.dll' back to 'eax.dll'.\n\n"
                            "To play CoopAndreas again:\n"
                            "Reinstall the mod.\n\nGetLastError() == %u",
                            dwError);
                        break;
                }

                MessageBox(0, msg, "CoopAndreas Loader", MB_OK | MB_ICONERROR);
            }
            break;
        case DLL_PROCESS_DETACH:
            if (!NeedToInjectCoopAndreas())
            {
                break;
            }
            if (hCoopAndreas)
            {
                FreeLibrary(hCoopAndreas);
            }
            break;
    }
    return TRUE;
}