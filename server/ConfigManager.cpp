#include "ConfigManager.h"

INIReader ConfigManager::reader(ConfigManager::configPath);

void ConfigManager::Init()
{
	if (!HasConfigExists())
	{
		CreateConfig();
	}
}

bool ConfigManager::HasConfigExists()
{
	std::ifstream file(configPath);
	return file.is_open();
}

void ConfigManager::CreateConfig()
{
	std::ofstream file(configPath);

	if (!file)
	{
		printf("[ERROR]: Couldn't create a server config file!");
		return;
	}

	for (auto& [key, value] : defaultConfig)
	{
		file << key << " = " << value << "\n";
	}
}

uint16_t ConfigManager::GetConfigPort()
{
	return (uint16_t)reader.GetUnsigned("", "port", defaultConfig.at("port"));
}

uint16_t ConfigManager::GetConfigMaxPlayers()
{
	return (uint16_t)reader.GetUnsigned("", "maxplayers", defaultConfig.at("maxplayers"));
}