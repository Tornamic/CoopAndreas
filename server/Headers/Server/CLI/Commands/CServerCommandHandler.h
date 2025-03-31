#if !defined(_COOPSERVER_SERVER_CLI_COMMANDS_SCOMMANDMANAGER_H_) || !defined(_COOPSERVER_SERVERCOMMANDMANAGER_H_)
#define _COOPSERVER_SERVER_CLI_COMMANDS_SCOMMANDMANAGER_H_
#define _COOPSERVER_SERVERCOMMANDMANAGER_H_

#include <iostream>
#include <cstdio>
#include <cstring>

static int MAX_SERVER_PLAYERS = 0;

namespace CServerCommandManager
{
	void ServerCommandInput(char *array[]);
	void ServerCommandHandler(char *array[]);	
}

namespace CServerCommand
{
	void Help();
}



#endif