#include "../core/CCommandManager.h"
#include "../core/CConfigFile.h"
#include "../core/CNetwork.h"
#include <cstring>
#include <thread>

void CCommandManager::Init(char* first_command, char* second_command, char* third_command, char* fourth_command)
{
  CCommandManager::ValueCheck(CCommandCall::ClearScreen(first_command));
  CCommandManager::ValueCheck(CCommandCall::StartServer(first_command));
  CCommandManager::ValueCheck(CCommandCall::StopServer(first_command));
}
void CCommandManager::ValueCheck(int returned_value)
{
    switch (returned_value) {
    case CCommandManager::COMMAND_SUCCESS: {
        // Print Here (Do Nothing I guess)
        break;
    }
    case CCommandManager::COMMAND_FAILS : {
        // Print Here An Error
      break;
    }
    case CCommandManager::COMMAND_UNKNOWN_ARGUMENTS: {
        break;
      }

    default: {
        break;
    }
    }
    return;
}

// CCommandCall Class Methods

int CCommandCall::ClearScreen(char *command_call) {
    
  if(strcmp(command_call, "cls") == 0 || strcmp(command_call, "clear") == 0)
    {
       for (unsigned short i = 0; i < 250; i++) {
       printf("\n");
    }
    }
    return CCommandManager::COMMAND_SUCCESS;
}

int CCommandCall::StartServer(char *command_call) {
    if (strcmp(command_call, "start-server") == 0 ||
        strcmp(command_call, "st-svr") == 0) {
      char ipaddress[DINI_MODULE_MAX_STRING_SIZE];
      int port = 0, maxconnections = 0;
      CConfigFile svr_config;
      svr_config.GeneralFunctionCall(ipaddress, port, maxconnections);
      unsigned short us_port = port;
      printf("\n[!] : Max number of players in server is %d\n", maxconnections);
      CNetwork::shared_loop_value = true;
      CNetwork::Init(ipaddress, us_port, maxconnections);
    }
    return CCommandManager::COMMAND_SUCCESS;
}

int CCommandCall::StopServer(char *command_call)
{
  if(strcmp(command_call, "stop-server") == 0 || strcmp(command_call, "stp-svr") == 0)
    {
      CNetwork::shared_loop_value = false;
      std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // wait untill thread finish      
    }
  return CCommandManager::COMMAND_SUCCESS;
}
