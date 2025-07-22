#include <iostream>
#include <cstring>
#include <string>

class CCommandManager
{
 public:
    static const int COMMAND_SUCCESS = 0;
    static const int COMMAND_FAILS = 1;
    static const int COMMAND_UNKNOWN_ARGUMENTS = 2;

    static inline bool loop_value = true;

    static void Init(char* first_command, char* second_command, char* third_command, char* fourth_command);
    static void ValueCheck(int returned_value); // checks if command returns values like FAILS or another value
};

class CCommandCall {
public:
    static int ClearScreen(char *command_call);
    static int StartServer(char *command_call);
    static int StopServer(char *command_call);
};
