#include "stdafx.h"
#include "CConfigLoader.h"

void CConfigLoader::BuildPath()
{
	ms_sDataPath = std::string((const char*)0xC92368) + "\\" + ms_sConfigName;
}

void CConfigLoader::BuildDefaultConfig()
{
    FILE* file = fopen(ms_sDataPath.c_str(), "wb");

	if (!file)
	{
		printf("Failed to create config file\n");
		return;
	}

    std::string config = "[" + std::string(ms_sConfigSection) + "]" + "\n";

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

void CConfigLoader::SetDirMyDocuments_Hook()
{
	BuildPath();
	Load();
	plugin::Call<0x538860>();
}

void CConfigLoader::Init()
{
	patch::RedirectCall(0x748995, SetDirMyDocuments_Hook);
}

void CConfigLoader::Load()
{
	EnsureCreated();

	GetPrivateProfileString(ms_sConfigSection, "nickname", "", CLocalPlayer::m_Name, sizeof(CLocalPlayer::m_Name), ms_sDataPath.c_str());
	GetPrivateProfileString(ms_sConfigSection, "ip", "", CNetwork::m_IpAddress, 15, ms_sDataPath.c_str());
	CNetwork::m_nPort = GetPrivateProfileInt(ms_sConfigSection, "port", Config::DEFAULT_PORT, ms_sDataPath.c_str());

#if DEBUG
	printf("%s %s %d\n", CLocalPlayer::m_Name, CNetwork::m_IpAddress, CNetwork::m_nPort);
#endif
}

void CConfigLoader::Save()
{
	EnsureCreated();

	WritePrivateProfileString(ms_sConfigSection, "nickname", CLocalPlayer::m_Name, ms_sDataPath.c_str());
	WritePrivateProfileString(ms_sConfigSection, "ip", CNetwork::m_IpAddress, ms_sDataPath.c_str());
	WritePrivateProfileString(ms_sConfigSection, "port", std::to_string(CNetwork::m_nPort).c_str(), ms_sDataPath.c_str());
}