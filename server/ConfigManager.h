#pragma once

#include <string>
#include <unordered_map>
#include <fstream>

#include "INIReader.h"
#include "core/CNetwork.h"

class ConfigManager
{
public:
	static INIReader reader;
	inline static const std::string configPath = "server-config.ini";

	inline static const std::unordered_map<std::string, uint16_t> defaultConfig = 
	{
		{"port", DEFAULT_SERVER_PORT},
		{"maxplayers", MAX_SERVER_PLAYERS}
	};

	static void Init();
	static bool HasConfigExists();
	static void CreateConfig();
	static uint16_t GetConfigPort();
	static uint16_t GetConfigMaxPlayers();
};