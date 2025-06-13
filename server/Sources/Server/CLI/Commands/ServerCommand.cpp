#include "../../../../Headers/Network/CNetwork.h"
#include "../../../../Headers/Server/CLI/Commands/ServerCommand.h"
#include <cstdlib>

int ServerCommand::HelpCommand(char *s0, char *s1, char *s2)
{
	printf("POOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOONG");
	return 0;
}

void ServerCommand::StartServerCommand(char *s0, std::promise<bool> p_var)
{
	short port = atoi(s0);
	CNetwork::Init(port, std::move(p_var));
}