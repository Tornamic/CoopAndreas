#if !defined(_COOPSERVER_CONFIG_FILE_H_) || !defined(_COOPSERVER_CCONFIGFILE_H_)
#define _COOPSERVER_CONFIG_FILE_H_
#define _COOPSERVER_CCONFIGFILE_H_  

// INIEngine-dini v0.1 , it works because i create it but it has a bit issues but it works
#include "../include/dini/iem-dracoblue-implementation.h"

using namespace INIEngine::Module::Dini;

class CConfigFile
{
	private:
		dini_file  	configfilehandle;
		dini_write 	config_writter;
		dini_read  	config_reader;
		dini_content config_content;
	public:
	
	char *configfilename = "coopserver.cfg";

	CConfigFile() { }

	bool InitConfigFile();
	bool GetConfigFileVariable_Port(int &value);
	bool GetConfigFileVariable_Players(int &value);
	bool GetConfigFileVariable_IPAddress(char value[]);

	void SetConfigFileVariable_Port(int &value);
	void SetConfigFileVariable_Players(int &value);
	void SetConfigFileVariable_IPAddress(char value[]);

	~CConfigFile() { }
};

#endif