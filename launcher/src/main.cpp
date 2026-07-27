#include <windows.h>
#include "../resource.h"
#include <CommCtrl.h>
#include "pcid.h"
#include "clipboard.h"
#include "launcher.h"
#include "registry.h"

#pragma comment(linker, \
    "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

std::string sSerial;
std::string sSerialCommand;

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
        {
            HINSTANCE hInstance = GetModuleHandle(NULL);

            HICON hIconSmall = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON,
                GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);

            HICON hIconBig = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON,
                GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);

            SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
            SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIconBig);

            SetDlgItemText(hDlg, ID_TEXTSERIALPC, sSerialCommand.c_str());

            char szSerial[64] = "";

            if (Registry::GetField(szSerial, sizeof(szSerial), "Serialkey"))
            {
                SetDlgItemText(hDlg, ID_EDITSERIALGENERATED, szSerial);
            }

            return (INT_PTR)TRUE;
        }
        case WM_COMMAND:
        {
            if (LOWORD(wParam) == ID_BTNCOPYSERIAL)
            {
                Clipboard::CopyText(sSerialCommand);
            }
            else if (LOWORD(wParam) == ID_BTNLAUNCH)
            {
                char szSerialGenerated[64];
                GetDlgItemText(hDlg, ID_EDITSERIALGENERATED, szSerialGenerated, 64);
                std::string cmd = "\"gta_sa.exe\" --coop -id " + sSerial + " -serial " + std::string(szSerialGenerated);
                Launcher::LaunchProcess(cmd);
            }
            else if (LOWORD(wParam) == ID_EDITSERIALGENERATED)
            {
                if (HIWORD(wParam) == EN_CHANGE)
                {
                    char szSerialGenerated[64];
                    GetDlgItemTextA(hDlg, ID_EDITSERIALGENERATED, szSerialGenerated, sizeof(szSerialGenerated));
                    Registry::SetField(szSerialGenerated, "Serialkey");
                }
            }
            break;
        }
        case WM_NOTIFY:
        {
            PNMLINK pNMLink = (PNMLINK)lParam;
            if (pNMLink->hdr.idFrom == IDC_LINKDISCORDSERVER)
            {
                if (pNMLink->hdr.code == NM_CLICK || pNMLink->hdr.code == NM_RETURN)
                {
                    ShellExecuteW(NULL, L"open", L"https://discord.gg/Z3ugSgFJMU", NULL, NULL, SW_SHOWNORMAL);
                    return (INT_PTR)TRUE;
                }
            }
            break;
        }
        case WM_CLOSE:
        {
            EndDialog(hDlg, 0);
            _exit(0);
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR)FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    wcscat_s(path, MAX_PATH, L".manifest");

    if (GetFileAttributesW(path) == INVALID_FILE_ATTRIBUTES)
    {
        wchar_t buffer[128 + MAX_PATH];
        wsprintfW(buffer,
            L"Could not find %s.\n\nCoopAndreas Launcher won't run.\n\nReinstall the mod to play CoopAndreas.", path);
        MessageBoxW(NULL, buffer, L"CoopAndreas Launcher Error", MB_ICONERROR | MB_OK);
        return 1;
    }

    ComputerID serialPC{};
    sSerial = serialPC.GetUniqueSystemId();
    sSerialCommand = "/gen " + sSerial;

    DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), nullptr, DialogProc, 0);
    return 0;
}
