#include "../../../../Headers/Server/CLI/Commands/ServerCommandManager.h"
#include "../../../../Headers/Server/CLI/Commands/ServerCommand.h"

void ServerCommandManager::CommandInput(char array[][32])
{
	printf("[>>] Enter The Command : ");
	scanf("%s %s %s %s", array[0], array[1], array[2], array[3]);
	ServerCommandManager::CommandHandler(array);
}

bool ServerCommandManager::CommandCheck(char *command, char *short_command, char *value_string)
{
	if(strcmp(command, value_string) == 0 || strcmp(short_command, value_string) == 0)
	{
		return true;
	}
	return false;
}

int ServerCommandManager::CommandResponse(int returned_value)
{
	if(returned_value == 1)
	{
		printf("Beta : Invalid Command Called");
	}
	return 0;
}


void ServerCommandManager::CommandHandler(char array[][32])
{
	printf("ServerCommandManager::CommandHandler %s", array[0]);
	if(ServerCommandManager::CommandCheck((char*)"--help", (char*)"-h", array[0]) == true)
	{
		ServerCommandManager::CommandResponse(ServerCommand::HelpCommand(array[1], array[2], array[3]));
	}
	else
	{
		printf("NOT SPECIFIED");
	}

}