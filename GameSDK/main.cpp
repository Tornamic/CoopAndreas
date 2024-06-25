#include "main.h"
#include <iostream>
DWORD WINAPI InitializeAndLoad(LPVOID) {
    while (*reinterpret_cast<unsigned char*>(0xC8D4C0) != 9) {
        Sleep(1);
    }
    *(float*)0x8A5E4C = 200000.0f; // increase max distance between 2 players
    InitScripting();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(hModule);
        CreateThread(0, 0, &InitializeAndLoad, 0, 0, 0);
        break;
    }
    case DLL_THREAD_DETACH: 
    {
        MessageBoxA(GetConsoleWindow(), "DLL_THREAD_DETACH", "C++Error", MB_OK);
        break;
    }
    case DLL_PROCESS_DETACH:
        MessageBoxA(GetConsoleWindow(), "DLL_PROCESS_DETACH", "C++Error", MB_OK);
        break;
    }
    return TRUE;
}