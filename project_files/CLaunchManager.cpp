#include "stdafx.h"

void CLaunchManager::CollectCommandLineArgs() // (C) ChatGPT 4-o
{
    LPSTR commandLine = GetCommandLineA();

    const int maxArgs = 200;
    char* args[maxArgs];
    int argc = 0;

    char* token = strtok(commandLine, " ");
    while (token != NULL && argc < maxArgs) {
        args[argc++] = token;
        token = strtok(NULL, " ");
    }

    char portStr[6] = "";

    for (int i = 0; i < argc; ++i) {
        if (strcmp(args[i], "-name") == 0 && (i + 1) < argc) {
            strcpy(CLocalPlayer::m_Name, args[i + 1]);
        }
        if (strcmp(args[i], "-ip") == 0 && (i + 1) < argc) {
            strcpy(CNetwork::m_IpAddress, args[i + 1]);
        }
        if (strcmp(args[i], "-port") == 0 && (i + 1) < argc) {
            strcpy(portStr, args[i + 1]);
        }
    }

    CNetwork::m_nPort = (unsigned short)atoi(portStr);

    printf("Player Nickname: %s\n", CLocalPlayer::m_Name);
    printf("Server IP Address: %s\n", CNetwork::m_IpAddress);
    printf("Server Port: %u\n", CNetwork::m_nPort);
}