#pragma once

#include <string>
#include <unordered_map>

#include "config.h"

class CConfigManager
{
public:
    inline static INIReader* ms_pReader = nullptr;
    inline static std::string ms_sConfigPath = "server-config.ini";

    inline static const std::unordered_map<std::string, uint16_t> ms_umDefaultConfig = {
        {"port", Config::DEFAULT_PORT}, {"maxplayers", Config::MAX_SERVER_PLAYERS}};

    static void Init();
    static bool HasConfigExists();
    static void CreateConfig();
    static uint16_t GetConfigPort();
    static uint16_t GetConfigMaxPlayers();
};