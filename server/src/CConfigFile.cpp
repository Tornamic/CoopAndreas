#include "../core/CConfigFile.h"

bool CConfigFile::InitConfigFile()
{
	if(CConfigFile::configfilehandle.FileExists(CConfigFile::configfilename) == false)
	{
		printf("[!] : Config file (%s) : Not Found , Doing Creation Operation ...\n", CConfigFile::configfilename);
		if(CConfigFile::configfilehandle.FileCreate(CConfigFile::configfilename) == false)
		{
			printf("\n[!] : Config file (%s) : Failed to create the file (for preventing NULL variables crashes and bugs i will exit)\n", CConfigFile::configfilename);
			return false;
		}
		else
		{
			config_content.CreateKey(CConfigFile::configfilename, "Server-Port");
			config_content.CreateKey(CConfigFile::configfilename, "Server-Players");
			config_content.CreateKey(CConfigFile::configfilename, "Server-IPAddress");
		}
	}
	return true;
}

bool CConfigFile::GetConfigFileVariable_Port(int &value)
{
	char *string;
	CConfigFile::config_reader.ReadString(CConfigFile::configfilename, "Server-Port", string);
	if(strcmp(string, "NULL") == 0)
	{
		printf("[!] : Config Port Key (Server-Port) : Server-Port value is not valid (for preventing NULL variables crashes and bugs i will exit)\n");
		return false;
	}
	CConfigFile::config_reader.ReadInt(CConfigFile::configfilename, "Server-Port", &value);
	printf("[!] : Config Port Key (Server-Port) : Server-Port value is %d \n", value);
	return true;
}

bool CConfigFile::GetConfigFileVariable_Players(int &value)
{
	char *string;
	CConfigFile::config_reader.ReadString(CConfigFile::configfilename, "Server-Players", string);
	if(strcmp(string, "NULL") == 0)
	{
		printf("\n[!] : Config Port Key (Server-Players) : Server-Port value is not valid (for preventing NULL variables crashes and bugs i will exit)\n");
		return false;
	}
	CConfigFile::config_reader.ReadInt(CConfigFile::configfilename, "Server-Players", &value);
	printf("\n[!] : Config Port Key (Server-Players) : Server-Players value is %d \n", value);
	return true;
}

bool CConfigFile::GetConfigFileVariable_IPAddress(char *value)
{
	char *string;
	CConfigFile::config_reader.ReadString(CConfigFile::configfilename, "Server-IPAddress", string);
	if(strcmp(string, "NULL") == 0)
	{
		printf("\n[!] : Config Port Key (Server-IPAddress) : Server-IPAddress value is not valid (for preventing NULL variables crashes and bugs i will exit)\n");
		return false;
	}
	CConfigFile::config_reader.ReadString(CConfigFile::configfilename, "Server-IPAddress", value);
	printf("\n[!] : Config Port Key (Server-IPAddress) : Server-IPAddress value is %d \n", value);
	return true;
}

void CConfigFile::SetConfigFileVariable_Port(int &value)
{
	CConfigFile::config_writter.WriteInt(CConfigFile::configfilename, "Server-Port", &value);
}

void CConfigFile::SetConfigFileVariable_Players(int &value)
{
	CConfigFile::config_writter.WriteInt(CConfigFile::configfilename, "Server-Players", &value);
}

void CConfigFile::SetConfigFileVariable_IPAddress(char *value)
{
	CConfigFile::config_writter.WriteString(CConfigFile::configfilename, "Server-IPAddress", value);
}
