#pragma once
#include <string>
#include <windows.h>

class Clipboard
{
public:
	static void CopyText(const std::string& text)
	{
        if (!OpenClipboard(NULL))
        {
            return;
        }

        EmptyClipboard();

        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
        if (!hMem)
        {
            CloseClipboard();
            return;
        }

        memcpy(GlobalLock(hMem), text.c_str(), text.size() + 1);
        GlobalUnlock(hMem);

        SetClipboardData(CF_TEXT, hMem);
        CloseClipboard();
	}
};