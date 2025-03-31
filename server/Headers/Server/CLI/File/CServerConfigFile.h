#if !defined(_COOPSERVER_SERVER_FILE_SCONFIGFILE_H_) || !defined(_COOPSERVER_SCONFIGFILE_H_)
#define _COOPSERVER_SERVER_FILE_SCONFIGFILE_H_
#define _COOPSERVER_SCONFIGFILE_H_

#include <iostream>

#include "../../../../include/INIEngine-Modules/dini.h"

using namespace INIEngine::Module::Dini;

namespace ServerConfigFile
{
	void ConfigFile(char *file);

	void StringOption_GetValue(char *key, char *value);
	void IntOption_GetValue(char *key, int *value);
	void FloatOption_GetValue(char *key, float *value);
	void BoolOption_GetValue(char *key, bool *value);

}


#endif