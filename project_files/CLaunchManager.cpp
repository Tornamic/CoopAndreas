#include "stdafx.h"

char* Encrypt(const char* id) {
    int length = std::strlen(id);

    char* stringBuilder = new char[11];
    std::memset(stringBuilder, 0, 11);
    int builderIndex = 0;

    for (int i = 0; i < length - 1; i += 2) 
    {
        int temp = id[i] + id[i + 1] + id[length - 1 - i];
        temp %= 256;

        if (temp % 3 == 0) 
        {
            stringBuilder[builderIndex++] = (char)((temp % 10) + '0');
        }
        else if (temp % 3 == 1) 
        {
            stringBuilder[builderIndex++] = (char)((temp % 26) + 'a');
        }
        else 
        {
            stringBuilder[builderIndex++] = (char)((temp % 26) + 'A');
        }
    }

    int halfLength = builderIndex / 2;

    for (int i = 1; i < length - 1 && builderIndex < 10; i++) 
    {
        int temp = id[i] + id[i + 1] + id[length - 1 - i];
        temp += stringBuilder[i / 2] + stringBuilder[halfLength] + stringBuilder[temp % builderIndex];

        if (temp % 3 == 0) 
        {
            stringBuilder[builderIndex++] = (char)((temp % 10) + '0');
        }
        else if (temp % 3 == 1) 
        {
            stringBuilder[builderIndex++] = (char)((temp % 26) + 'a');
        }
        else 
        {
            stringBuilder[builderIndex++] = (char)((temp % 26) + 'A');
        }
    }

    stringBuilder[builderIndex] = '\0';

    return stringBuilder;
}

void CLaunchManager::CollectCommandLineArgs()
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
    char serial[11] = "";
    char id[9] = "";

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
        if (strcmp(args[i], "-serial") == 0 && (i + 1) < argc) {
            strncpy(serial, args[i + 1], 10);
            serial[10] = '\0';
        }
        if (strcmp(args[i], "-id") == 0 && (i + 1) < argc) {
            strncpy(id, args[i + 1], 8);
            id[8] = '\0';
        }
    }

    CNetwork::m_nPort = (unsigned short)atoi(portStr);

#ifdef _DEV
    if (CLocalPlayer::m_Name[0] == '\0') {
        strcpy(CLocalPlayer::m_Name, "Tornamic");
        strcpy(CNetwork::m_IpAddress, "127.0.0.1");
        CNetwork::m_nPort = 6767;
        return;
    }

    printf("Player Nickname: %s\n", CLocalPlayer::m_Name);
    printf("Server IP Address: %s\n", CNetwork::m_IpAddress);
    printf("Server Port: %u\n", CNetwork::m_nPort);
#endif // _DEV

    char* encrypted = Encrypt(id);

    if (!serial[0] || !id[0] || strcmp(encrypted, serial) != 0)
    {
        MessageBoxA(0, "You have entered an incorrect serial key, please check that you have entered it correctly.", "Failed to verify CoopAndreas serial key", MB_OK | MB_ICONERROR);
            
        // crash the game
        *(ushort*)0xDEAD = 0xDEAD;
    }

    delete[] encrypted;
}
