#pragma once

#include <string>
#include <unordered_map>

#include "INIReader/cpp/INIReader.h"
#include "CNetwork.h"
#include <config.h>

class CConfigManager
{
public:
	static inline INIReader* ms_pReader = nullptr;
	static inline std::string ms_sConfigPath = "server-config.ini";

	static inline const std::unordered_map<std::string, uint16_t> ms_umDefaultConfig =
	{
		{"port", Config::DEFAULT_PORT},
		{"maxplayers", MAX_SERVER_PLAYERS}
	};

	static void Init();
	static bool HasConfigExists();
	static void CreateConfig();
	static uint16_t GetConfigPort();
	static uint16_t GetConfigMaxPlayers();
};