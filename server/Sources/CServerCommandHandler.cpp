#include "../core/CServerCommandHandler.h"

namespace CServerCommandManager  
{	
	void ServerCommandInput(char *array[])
	{
		printf("\n[>>] Type a Command (cmd sub-cmd sub-cmd sub-cmd) :");
		scanf("%s %s %s %s", array[0], array[1], array[2], array[3]);
		ServerCommandHandler(array);
	}

	void ServerCommandHandler(char *array[])
	{
		if(strcmp(array[0], "help") == 0 || strcmp(array[0], "-h") == 0)
		{
			CServerCommand::Help();
		}
		else
			printf("Error No Valid Command");
	}

}

namespace CServerCommand
{
	void Help()
	{
		printf("Ping !\n");
	}
}