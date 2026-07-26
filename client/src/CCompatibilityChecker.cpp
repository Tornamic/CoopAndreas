#include "CCompatibilityChecker.h"

void CCompatibilityChecker::Process()
{
    static uint32_t lastChecked = 0;
    // check every 2 secs
    if (GetTickCount() - lastChecked <= 2000)
    {
        return;
    }
    lastChecked = GetTickCount();

    /*
    CLEO+ overrides CWeapon::DoBulletImpact and makes player weapon shots desynced
    */
    static bool bIgnoreCleoPlus = false;
    if (!bIgnoreCleoPlus)
    {
        HMODULE hCleoPlus = GetModuleHandle("CLEO+.cleo");
        if (hCleoPlus)
        {
            char cleoPlusPath[MAX_PATH];
            GetModuleFileName(hCleoPlus, cleoPlusPath, MAX_PATH);

            static char buffer[512 + MAX_PATH];
            sprintf(buffer,
                "Incompatible modification detected: CLEO+ (CLEO+.cleo)\n\n"
                "This module conflicts with CoopAndreas.\n\n"
                "Side effect: other players will not see your weapon shots.\n\n"
                "Please remove CLEO+ (%s) and restart the game.\n\n"
                "Do you want to continue anyway? (You will be playing a buggy game)",
                cleoPlusPath);

            ShowCursor(true);
            if (MessageBoxA(nullptr, buffer, "CoopAndreas Compatibility Warning", MB_ICONWARNING | MB_YESNO) == IDNO)
            {
                ExitProcess(0);
            }
            else
            {
                bIgnoreCleoPlus = true;
            }
        }
    }

    /*
    CLEO5 reimplements CRunningScript::ReadTextLabelFromScript and
    our hooks can't collect text arguments from an opcode to sync
    */
    static bool bIgnoreCleo5 = false;
    if (!bIgnoreCleo5)
    {
        HMODULE hCleo = GetModuleHandle("CLEO.asi");
        if (hCleo)
        {
            auto CLEO_GetVersion = (DWORD(WINAPI*)())GetProcAddress(hCleo, "_CLEO_GetVersion@0");

            if (CLEO_GetVersion)
            {
                DWORD cleoVersion = CLEO_GetVersion();
                if (cleoVersion >= 0x05000000)  // CLEO5
                {
                    char cleoPath[MAX_PATH];
                    GetModuleFileName(hCleo, cleoPath, MAX_PATH);

                    char cleoVerStr[32];

                    sprintf(cleoVerStr, "%u.%u.%u", (cleoVersion >> 24) & 0xFF, (cleoVersion >> 16) & 0xFF,
                        (cleoVersion >> 8) & 0xFF);

                    static char buffer[512 + MAX_PATH];
                    sprintf(buffer,
                        "Incompatible modification detected: CLEO5 v%s (CLEO.asi)\n\n"
                        "CLEO5 conflicts with CoopAndreas.\n\n"
                        "Side effect: most missions will be unplayable.\n\n"
                        "Please remove CLEO (%s) or downgrade to CLEO 4, then restart the game.\n\n"
                        "Do you want to continue anyway? (You will be playing a buggy game)",
                        cleoVerStr, cleoPath);

                    ShowCursor(true);
                    if (MessageBoxA(nullptr, buffer, "CoopAndreas Compatibility Warning", MB_ICONWARNING | MB_YESNO) ==
                        IDNO)
                    {
                        ExitProcess(0);
                    }
                    else
                    {
                        bIgnoreCleo5 = true;
                    }
                }
            }
        }
    }
}
