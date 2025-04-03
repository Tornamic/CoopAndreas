#include "../core/CServerConfigFile.h"

namespace ServerConfigFile
{
	void ConfigFile(dini_file file, dini_read f_reader, char *file_name)
	{
		if(file.FileExists(file_name) == true)
		{
			

		}
		else
			printf("Invalid or missong CFG file\n");
	}
}