#if !defined(_COOPSERVER_SERVER_CLI_COMMANDS_SCOMMANDMANAGER_H_) || !defined(_COOPSERVER_SERVERCOMMANDMANAGER_H_)
#define _COOPSERVER_SERVER_CLI_COMMANDS_SCOMMANDMANAGER_H_
#define _COOPSERVER_SERVERCOMMANDMANAGER_H_

#include <iostream>
#include <cstdio>
#include <cstring>


class ServerCommandManager
{
public:
	static void CommandInput(char array[][32]);
	static void CommandHandler(char array[][32]);	
	static bool CommandCheck(char *command, char *short_command, char *value_string);
	static int  CommandResponse(int returned_value);
};


#endif