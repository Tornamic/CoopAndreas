#if !defined(_COOP_SERVER_CSERVERCOMMANDMANAGER_H_)
#define _COOP_SERVER_CSERVERCOMMANDMANAGER_H_

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