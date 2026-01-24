#include "stdafx.h"
#include "CConfigLoader.h"
#include "CCustomMenuManager.h"

void CConfigLoader::BuildPath()
{
	ms_sDataPath = std::string((const char*)0xC92368) + "\\" + CONFIG_FILE_NAME; // 0xC92368 - gta_user_dir_path var
}

void CConfigLoader::BuildDefaultConfig()
{
    FILE* file = fopen(ms_sDataPath.c_str(), "wb");

	if (!file)
	{
		printf("Failed to create config file\n");
		return;
	}

    std::string config = "[" + std::string(CONFIG_SECTION) + "]" + "\n";

	for (const auto& [key, value] : ms_umDefaultConfig)
	{
		config += key + "=" + value + "\n";
	}

    fwrite(config.c_str(), sizeof(char), config.length(), file);
    fclose(file);
}

void CConfigLoader::EnsureCreated()
{
	if (FileExists(ms_sDataPath.c_str()))
	{
		return;
	}

	BuildDefaultConfig();
}

uintptr_t SetDirMyDocuments_Hook_ptr = 0x0;
void CConfigLoader::SetDirMyDocuments_Hook()
{
	BuildPath();
	Load();
	CCustomMenuManager::UpdateFromConfig();
	plugin::CallDyn(SetDirMyDocuments_Hook_ptr);
}

void CConfigLoader::Init()
{
	SetDirMyDocuments_Hook_ptr = injector::GetBranchDestination(0x748995).as_int();
	patch::RedirectCall(0x748995, SetDirMyDocuments_Hook);
}

void CConfigLoader::Load()
{
	EnsureCreated();

	GetPrivateProfileString(CONFIG_SECTION, "nickname", "", CLocalPlayer::m_Name, sizeof(CLocalPlayer::m_Name), ms_sDataPath.c_str());
	GetPrivateProfileString(CONFIG_SECTION, "ip", "", CNetwork::m_IpAddress, 15, ms_sDataPath.c_str());
	CNetwork::m_nPort = GetPrivateProfileInt(CONFIG_SECTION, "port", Config::DEFAULT_PORT, ms_sDataPath.c_str());

#if DEBUG
	printf("%s %s %d\n", CLocalPlayer::m_Name, CNetwork::m_IpAddress, CNetwork::m_nPort);
#endif
}

void CConfigLoader::Save()
{
	EnsureCreated();
	
	WritePrivateProfileString(CONFIG_SECTION, "nickname", CLocalPlayer::m_Name, ms_sDataPath.c_str());
	WritePrivateProfileString(CONFIG_SECTION, "ip", CNetwork::m_IpAddress, ms_sDataPath.c_str());
	WritePrivateProfileString(CONFIG_SECTION, "port", std::to_string(CNetwork::m_nPort).c_str(), ms_sDataPath.c_str());
}