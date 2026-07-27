#pragma once
#include <windows.h>

class Registry
{
public:
    static bool GetField(char* buffer, DWORD size, const char* field)
    {
        HKEY hKey;
        DWORD type = REG_SZ;

        if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\CoopAndreas", 0, KEY_READ, &hKey) != ERROR_SUCCESS)
            return false;

        LONG result = RegQueryValueExA(hKey, field, NULL, &type, reinterpret_cast<LPBYTE>(buffer), &size);

        RegCloseKey(hKey);

        return result == ERROR_SUCCESS && type == REG_SZ;
    }

    static void SetField(const char* value, const char* field)
    {
        HKEY hKey;

        if (RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\CoopAndreas", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE,
                NULL, &hKey, NULL) == ERROR_SUCCESS)
        {
            RegSetValueExA(hKey, field, 0, REG_SZ, (const BYTE*)value, (DWORD)strlen(value) + 1);
            RegCloseKey(hKey);
        }
    }
};
