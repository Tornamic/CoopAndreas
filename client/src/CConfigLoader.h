#pragma once

class CConfigLoader
{
private:
	static void BuildPath();
	static void BuildDefaultConfig();
	static void SetDirMyDocuments_Hook();
	static void EnsureCreated();
	static void Load();

	static inline std::string ms_sDataPath = "";

	static inline std::unordered_map<std::string, std::string> ms_umDefaultConfig =
	{
		{"nickname", ""},
		{"ip", ""},
		{"port", std::to_string(Config::DEFAULT_PORT)}
	};;
	
public:
	static inline const char* CONFIG_SECTION = "config";
	static inline const char* CONFIG_FILE_NAME = "coopandreas.ini";

	static void Init();
	static void Save();
};