#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <fstream>
#include <filesystem>

#if defined(_WIN32)
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#endif

#include "enet/enet.h"
#include "thirdparty-libraries/INIReader/cpp/INIReader.h"

#include "core/CControllerState.h"
#include "core/NetworkEntityType.h"
#include "core/CPacketListener.h"
#include "core/CVector.h"
#include "core/CNetwork.h"
#include "core/CPed.h"
#include "core/CPedManager.h"
#include "core/CPlayer.h"
#include "core/CPlayerManager.h"
#include "core/CVehicle.h"
#include "core/CVehicleManager.h"
#include "core/VehicleDoorState.h"
#include "core/ConfigManager.h"

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

std::filesystem::path GetExecutableDir()
{
#if defined(_WIN32)
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    return std::filesystem::path(buffer).parent_path();
#elif defined(__linux__)
    char buffer[4096];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1)
    {
        buffer[len] = '\0';
        return std::filesystem::path(buffer).parent_path();
    }
    return std::filesystem::current_path();
#endif
}

int main(int argc, char* argv[])
{
#if defined (_WIN32)
    SetConsoleTitleW(L"CoopAndreas Server");
#endif

    std::filesystem::path exeDir = GetExecutableDir();
    std::error_code ec;
    std::filesystem::current_path(exeDir, ec);

    printf("[!] : Support:\n");
    printf("- https://github.com/Tornamic/CoopAndreas\n");
    printf("- https://discord.gg/TwQsR4qxVx\n");
    printf("- coopandreasmod@gmail.com\n\n");

    printf("[!] : CoopAndreas Server \n");
#ifdef _DEBUG
    char config[] = "Debug";
#else
    char config[] = "Release";
#endif

    printf("[!] : Version : %s, %s %s %s %s\n", COOPANDREAS_VERSION, config, sizeof(void*) == 8 ? "x64" : "x86", __DATE__, __TIME__);
#if defined (_WIN32)
    printf("[!] : Platform : Microsoft Windows \n");
#else
    printf("[!] : Platform : GNU/Linux | BSD \n");
#endif

    CConfigManager::Init();

    CNetwork::Init(CConfigManager::GetConfigPort());
    return 0;
}