#include "stdafx.h"
#include "CCrashLog.h"
#include <windows.h>
#include <dbghelp.h>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <chrono>

#pragma comment(lib, "dbghelp.lib")

void CopyToClipboard(const std::string& text)
{
    // open the clipboard
    if (OpenClipboard(NULL))
    {
        // empty the clipboard
        EmptyClipboard();

        // allocate global memory for the text
        HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
        if (hGlob)
        {
            // copy the text to the global memory
            memcpy(GlobalLock(hGlob), text.c_str(), text.size() + 1);
            GlobalUnlock(hGlob);

            // set the clipboard data
            SetClipboardData(CF_TEXT, hGlob);
        }

        // close the clipboard
        CloseClipboard();
    }
}

std::string GetBacktrace(CONTEXT* ctx) {
    HANDLE process = GetCurrentProcess();
    SymInitialize(process, NULL, TRUE);
    STACKFRAME64 stackFrame = {};
    stackFrame.AddrPC.Offset = ctx->Eip;
    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Offset = ctx->Ebp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Offset = ctx->Esp;
    stackFrame.AddrStack.Mode = AddrModeFlat;

    DWORD imageType = IMAGE_FILE_MACHINE_I386;
    SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    for (int i = 0; i < 32; ++i) {
        if (!StackWalk64(imageType, process, GetCurrentThread(), &stackFrame, ctx, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL) || stackFrame.AddrPC.Offset == 0) {
            break;
        }

        if (SymFromAddr(process, stackFrame.AddrPC.Offset, 0, symbol)) {
            ss << "    " << i << ": " << symbol->Name << " - 0x" << std::setw(8) << symbol->Address << "\n";
        }
        else {
            ss << "    " << i << ": (no symbol - 0x" << std::setw(8) << stackFrame.AddrPC.Offset << "\n";
        }
    }

    free(symbol);
    SymCleanup(process);
    return ss.str();
}

LONG __stdcall CCrashLog::ExceptionHandler(_EXCEPTION_POINTERS* exceptionInfo) {
    DWORD exceptionAddress = (DWORD)exceptionInfo->ExceptionRecord->ExceptionAddress;
    DWORD code = exceptionInfo->ExceptionRecord->ExceptionCode;

    std::stringstream ss;
    ss << std::hex << code << ": unhandled exception at 0x" << std::hex << exceptionAddress << ".\n\n";
    ss << "Register dump:\n";
    CONTEXT* ctx = exceptionInfo->ContextRecord;

    for (const auto& reg : { std::make_pair("EAX", ctx->Eax), std::make_pair("EBX", ctx->Ebx), std::make_pair("ECX", ctx->Ecx),
                              std::make_pair("EDX", ctx->Edx), std::make_pair("EDI", ctx->Edi), std::make_pair("ESI", ctx->Esi),
                              std::make_pair("EBP", ctx->Ebp), std::make_pair("EIP", ctx->Eip), std::make_pair("ESP", ctx->Esp),
                              std::make_pair("EFL", ctx->EFlags), std::make_pair("CS", ctx->SegCs), std::make_pair("SS", ctx->SegSs),
                              std::make_pair("GS", ctx->SegGs), std::make_pair("FS", ctx->SegFs), std::make_pair("ES", ctx->SegEs),
                              std::make_pair("DS", ctx->SegDs) }) {
        ss << "    " << reg.first << ": 0x" << std::hex << std::setw(8) << std::setfill('0') << reg.second << "\n";
    }

    ss << "\n\nBacktrace:\n" << GetBacktrace(ctx) << '\n';
    ss << "Save crashlog to the clipboard?";
    if (MessageBoxA(*(HWND*)0xC97C1C, ss.str().c_str(), ";(", MB_ICONERROR | MB_YESNO) == IDYES)
    {
        CopyToClipboard(ss.str());
    }
    return 0;
}