#include "stdafx.h"
#include "CConfigLoader.h"
#include "CFileMgr.h"

void EnsureCreated()
{
	//CFileMgr::SetDir("");
	if (!FileExists("coopandreas.ini"))
	{
		char dir[MAX_PATH];
		GetCurrentDirectoryA(sizeof(dir), dir);
		std::string path = std::string(dir) + "\\coopandreas.ini";

		FILE* file = fopen("coopandreas.ini", "wb");
		const char* stub =
			"[config]\n"
			"nickname=\n"
			"ip=\n"
			"port=6767\n";
		fwrite(stub, sizeof(char), strlen(stub), file);
		fclose(file);
	}
}
void CConfigLoader::Load()
{
	EnsureCreated();

	char dir[MAX_PATH];
	GetCurrentDirectoryA(sizeof(dir), dir);
	std::string path = std::string(dir) + "\\coopandreas.ini";

	GetPrivateProfileString("config", "nickname", "", CLocalPlayer::m_Name, 32, path.c_str());
	GetPrivateProfileString("config", "ip", "", CNetwork::m_IpAddress, 15, path.c_str());
	CNetwork::m_nPort = GetPrivateProfileInt("config", "port", 6767, path.c_str());

	printf("%s %s %d", CLocalPlayer::m_Name, CNetwork::m_IpAddress, CNetwork::m_nPort);
}

void CConfigLoader::Save()
{
	EnsureCreated();

	char dir[MAX_PATH];
	GetCurrentDirectoryA(sizeof(dir), dir);
	std::string path = std::string(dir) + "\\coopandreas.ini";

	WritePrivateProfileString("config", "nickname", CLocalPlayer::m_Name, path.c_str());
	WritePrivateProfileString("config", "ip", CNetwork::m_IpAddress, path.c_str());
	WritePrivateProfileString("config", "port", std::to_string(CNetwork::m_nPort).c_str(), path.c_str());
}