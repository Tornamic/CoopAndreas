#include "stdafx.h"
#include "CCrashLog.h"
#include <windows.h>
#include <dbghelp.h>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <chrono>
#include <resources.h>
#include <Psapi.h>
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "dbghelp.lib")

static void WriteDumpf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    size_t len = strlen(CCrashLog::ms_szCrashMessage);
    std::vsnprintf(CCrashLog::ms_szCrashMessage + len, sizeof(CCrashLog::ms_szCrashMessage) - len, fmt, args);
    va_end(args);
}

static void AppendCrashMessage(DWORD code, const char* description)
{
    WriteDumpf("0x%X: %s", code, description);
}

void GetModuleName(HMODULE moduleHandle, char* buffer)
{
    if (GetModuleFileNameA(moduleHandle, buffer, 260))
    {
        char* lastSlash = strrchr(buffer, '\\');

        if (!lastSlash)
            lastSlash = strrchr(buffer, '/');

        if (lastSlash)
        {
            const char* fileName = lastSlash + 1;
            size_t length = strlen(fileName);
            memmove(buffer, fileName, length);
            buffer[length] = '\0';
        }
        return;
    }

    strcpy(buffer, "unknown");
}

void WriteBacktrace(CONTEXT* ctx) {
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
    if (!symbol)
    {
        WriteDumpf("   Can't generate backtrace: symbol == nullptr");
        return;
    }

    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    for (int i = 0; i < 32; ++i) 
    {
        if (!StackWalk64(imageType, process, GetCurrentThread(), &stackFrame, ctx, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL) || stackFrame.AddrPC.Offset == 0) 
        {
            break;
        }

        HMODULE baseAddress = NULL;
        GetModuleHandleEx(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCTSTR)symbol->Address,
            &baseAddress);

        char moduleName[268];
        GetModuleName(baseAddress, moduleName);

        if (SymFromAddr(process, stackFrame.AddrPC.Offset, 0, symbol)) 
        {
            WriteDumpf("   0x%p: %s in %s (+0x%x) (0x%p)\r\n", (void*)symbol->Address, symbol->Name, moduleName, (DWORD)symbol->Address - (DWORD)baseAddress, stackFrame.AddrPC.Offset);
        }
        else 
        {
            WriteDumpf("   0x%p: sub_%p in %s (+0x%x)\r\n", (void*)symbol->Address, (void*)stackFrame.AddrPC.Offset, moduleName, (DWORD)symbol->Address - (DWORD)baseAddress);
        }
    }

    free(symbol);
    SymCleanup(process);
}


void WriteExceptionText(_EXCEPTION_POINTERS* exceptionInfo)
{
    DWORD code = exceptionInfo->ExceptionRecord->ExceptionCode;
    bool extraInfo = false;
    bool inPageError = false;

    WriteDumpf("\r\n");

    static const std::unordered_map<DWORD, const char*> exceptionDescriptions = {
        { 0xC0000005, "Access violation" },
        { 0xC0000006, "In page error" },
        { 0xC000001D, "Illegal instruction" },
        { 0xC0000025, "Non-continuable exception" },
        { 0xC0000026, "Invalid disposition" },
        { 0xC000008C, "Array bounds exceeded" },
        { 0xC000008D, "Denormal float operand" },
        { 0xC000008E, "Floating-point division by zero" },
        { 0xC000008F, "Floating-point inexact result" },
        { 0xC0000090, "Floating-point invalid operation" },
        { 0xC0000091, "Floating-point overflow" },
        { 0xC0000092, "Floating-point stack check" },
        { 0xC0000093, "Floating-point underflow" },
        { 0xC0000094, "Integer division by zero" },
        { 0xC0000095, "Integer overflow" },
        { 0xC0000096, "Privileged instruction" },
        { 0xC00000FD, "Stack overflow" },
        { 0x80000002, "Data type misalignment exception" },
        { 0x80000003, "Breakpoint exception" },
        { 0x80000004, "Single step exception" }
    };

    auto it = exceptionDescriptions.find(code);
    if (it != exceptionDescriptions.end()) {
        AppendCrashMessage(code, it->second);
        if (code == 0xC0000005 || code == 0xC0000006)
            extraInfo = true;
        if (code == 0xC0000006)
            inPageError = true;

        if (code >= 0x80000002 && code <= 0x80000004)
        {
            WriteDumpf(".");
            return;
        }
    }
    else {
        AppendCrashMessage(code, "NO_DESCRIPTION");
    }

    if (extraInfo) {
        ULONG_PTR* info = exceptionInfo->ExceptionRecord->ExceptionInformation;
        void* address = (void*)info[1];
        const char* accessType = "Unknown access";

        if (inPageError) {
            accessType = "In-page error access";
        }
        else {
            switch (info[0]) {
            case 0: accessType = " read"; break;
            case 1: accessType = " write"; break;
            case 8: accessType = " execute"; break;
            }
        }

        WriteDumpf("%s", accessType);
    }
}

static INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG:
    {
        ShowCursor(TRUE);
        SetDlgItemText(hDlg, IDC_CRASHLOG, CCrashLog::ms_szCrashMessage);
        
        if (CCrashLog::ms_bSuccessSavedLog)
        {
            char filename[260+20] = "Crash log saved to ";
            sprintf_s(filename, "%sCoopAndreas_crashes\\%04d-%02d-%02d_%02d-%02d-%02d.log", filename,
                CCrashLog::ms_exceptionTime.tm_year + 1900, CCrashLog::ms_exceptionTime.tm_mon + 1, CCrashLog::ms_exceptionTime.tm_mday,
                CCrashLog::ms_exceptionTime.tm_hour, CCrashLog::ms_exceptionTime.tm_min, CCrashLog::ms_exceptionTime.tm_sec);
            SetDlgItemText(hDlg, IDC_SAVEDTO, filename);
        } 
        else
        {
            SetDlgItemText(hDlg, IDC_SAVEDTO, "Failed to create a log file");
        }

        PROCESS_MEMORY_COUNTERS_EX pmc;
        DWORD processID = GetCurrentProcessId();
        HANDLE hProcess = GetCurrentProcess();

        if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) 
        {
            SetDlgItemText(hDlg, IDC_SAVEDUMP, ("Save Dump (~" + std::to_string(pmc.WorkingSetSize / 1024 / 1024) + "MB)").c_str());
        }

        return (INT_PTR)TRUE;
    }
    case WM_COMMAND:
    {
        if (LOWORD(wParam) == IDC_EXIT) 
        {
            EndDialog(hDlg, 0);
            _exit(0);
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDC_SAVEDUMP)
        {
            CreateDirectoryA("CoopAndreas_crashes", NULL);

            char filename[260];
            sprintf_s(filename, "CoopAndreas_crashes\\%04d-%02d-%02d_%02d-%02d-%02d.dmp",
                CCrashLog::ms_exceptionTime.tm_year + 1900, CCrashLog::ms_exceptionTime.tm_mon + 1, CCrashLog::ms_exceptionTime.tm_mday,
                CCrashLog::ms_exceptionTime.tm_hour, CCrashLog::ms_exceptionTime.tm_min, CCrashLog::ms_exceptionTime.tm_sec);

            HANDLE hFile = CreateFileA(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if (hFile != INVALID_HANDLE_VALUE)
            {
                MINIDUMP_EXCEPTION_INFORMATION mdei;
                mdei.ThreadId = GetCurrentThreadId();
                mdei.ExceptionPointers = CCrashLog::ms_pExceptionPointers;
                mdei.ClientPointers = FALSE;

                MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpWithFullMemory, &mdei, NULL, NULL);

                CloseHandle(hFile);

                SetDlgItemText(hDlg, IDC_SAVEDTO, ("Memory dump saved to " + std::string(filename)).c_str());
            }
            return (INT_PTR)TRUE;
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

LONG __stdcall CCrashLog::ExceptionHandler(_EXCEPTION_POINTERS* exceptionInfo) {
    ms_pExceptionPointers = exceptionInfo;

    time_t now = time(nullptr);
    localtime_s(&ms_exceptionTime, &now);

    DWORD exceptionAddress = (DWORD)exceptionInfo->ExceptionRecord->ExceptionAddress;
    DWORD code = exceptionInfo->ExceptionRecord->ExceptionCode;

    memset(ms_szCrashMessage, 0, sizeof(ms_szCrashMessage));

    HMODULE baseAddress = NULL;
    GetModuleHandleEx(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCTSTR)exceptionInfo->ExceptionRecord->ExceptionAddress,
        &baseAddress);

    char moduleName[268];
    GetModuleName(baseAddress, moduleName);

    CCore::gvm.Detect();
    CCore::gvm.GetVersionText(ms_szCrashMessage);
    WriteDumpf(", CoopAndreas v%s %s %s", COOPANDREAS_VERSION, __DATE__, __TIME__);

    WriteDumpf("\r\nUnhandled exception at 0x%p in %s", (void*)exceptionAddress, moduleName);
    if (baseAddress)
    {
        WriteDumpf(" (+0x%p)", (void*)(exceptionAddress - (DWORD)baseAddress));
    }

    WriteExceptionText(exceptionInfo);

    WriteDumpf("\r\n\r\n");

    WriteDumpf("Register dump:\r\n");
    WriteDumpf("   EAX: 0x%p  ", (void*)exceptionInfo->ContextRecord->Eax);
    WriteDumpf("   EBX: 0x%p  ", (void*)exceptionInfo->ContextRecord->Ebx);
    WriteDumpf("   ECX: 0x%p  ", (void*)exceptionInfo->ContextRecord->Ecx);
    WriteDumpf("   EDX: 0x%p  ", (void*)exceptionInfo->ContextRecord->Edx);
    WriteDumpf("\r\n");
    WriteDumpf("   ESI: 0x%p  ", (void*)exceptionInfo->ContextRecord->Esi);
    WriteDumpf("   EDI: 0x%p  ", (void*)exceptionInfo->ContextRecord->Edi);
    WriteDumpf("   EBP: 0x%p  ", (void*)exceptionInfo->ContextRecord->Ebp);
    WriteDumpf("   ESP: 0x%p  ", (void*)exceptionInfo->ContextRecord->Esp);
    WriteDumpf("\r\n");
    WriteDumpf("   EIP: 0x%p  ", (void*)exceptionInfo->ContextRecord->Eip);
    WriteDumpf("   EFL: 0x%p  ", (void*)exceptionInfo->ContextRecord->EFlags);
    WriteDumpf("\r\n");
    WriteDumpf("   CS:  0x%p  ", (void*)exceptionInfo->ContextRecord->SegCs);
    WriteDumpf("   SS:  0x%p  ", (void*)exceptionInfo->ContextRecord->SegSs);
    WriteDumpf("\r\n");
    WriteDumpf("   GS:  0x%p  ", (void*)exceptionInfo->ContextRecord->SegGs);
    WriteDumpf("   FS:  0x%p  ", (void*)exceptionInfo->ContextRecord->SegFs);
    WriteDumpf("\r\n");
    WriteDumpf("   ES:  0x%p  ", (void*)exceptionInfo->ContextRecord->SegEs);
    WriteDumpf("   DS:  0x%p  ", (void*)exceptionInfo->ContextRecord->SegDs);
    WriteDumpf("\r\n\r\n");


    WriteDumpf("Stack dump:\r\n");

    MEMORY_BASIC_INFORMATION mbi;
    const void* lpAddress = (const void*)exceptionInfo->ContextRecord->Esp;
    const void* AllocationBase = nullptr;

    if (VirtualQuery(lpAddress, &mbi, sizeof(mbi)) && (mbi.State & MEM_COMMIT) && (mbi.Protect & PAGE_READWRITE)) {
        AllocationBase = mbi.AllocationBase;

        const void** stackPointer = (const void**)lpAddress;
        const void** stackTop = (const void**)((char*)mbi.BaseAddress + mbi.RegionSize);

        
        int count = 0;
        while (stackPointer < stackTop && count < 180) {
            if (count % 6 == 0) {
                if (count != 0) WriteDumpf("\r\n");
                WriteDumpf("   0x%p: ", stackPointer);
            }
            WriteDumpf(" %p", *stackPointer++);
            count++;
        }
        WriteDumpf("\r\n");
        WriteDumpf("   base: 0x%p   top: 0x%p   bottom: 0x%p", AllocationBase, lpAddress, stackTop);
    }

    WriteDumpf("\r\n\r\n");
    WriteDumpf("Backtrace:");
    WriteDumpf("\r\n");
    WriteBacktrace(exceptionInfo->ContextRecord);

    WriteDumpf("\r\n\r\nLoaded modules:\r\n");

    HMODULE hMods[1024];
    DWORD cbNeeded;
    HANDLE hProcess = GetCurrentProcess();

    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
        {
            TCHAR szModName[MAX_PATH];
            if (GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
            {
                DWORD dummy;
                DWORD size = GetFileVersionInfoSize(szModName, &dummy);
                std::string versionStr = "N/A";

                if (size > 0)
                {
                    std::vector<BYTE> data(size);
                    if (GetFileVersionInfo(szModName, 0, size, data.data()))
                    {
                        VS_FIXEDFILEINFO* verInfo = nullptr;
                        UINT len = 0;
                        if (VerQueryValue(data.data(), TEXT("\\"), (LPVOID*)&verInfo, &len))
                        {
                            if (verInfo)
                            {
                                char buf[64];
                                sprintf_s(buf, "%u.%u.%u.%u",
                                    (verInfo->dwFileVersionMS >> 16) & 0xFFFF,
                                    (verInfo->dwFileVersionMS >> 0) & 0xFFFF,
                                    (verInfo->dwFileVersionLS >> 16) & 0xFFFF,
                                    (verInfo->dwFileVersionLS >> 0) & 0xFFFF);
                                versionStr = buf;
                            }
                        }
                    }
                }

                WriteDumpf("   Base: 0x%p  Module: %s  Version: %s\r\n", hMods[i], szModName, versionStr.c_str());
            }
        }
    }


    WriteDumpf("\r\n");
    WriteDumpf("Active scripts:\r\n");
    for (auto* script = CTheScripts::pActiveScripts; script; script = script->m_pNext) 
    {
        if (!script->m_bIsActive) 
        {
            continue;
        }

        WriteDumpf("   %-8s   BIP %08X   CIP %08X   %d%d%d%d\r\n",
            script->m_szName,
            (unsigned int)script->m_pBaseIP,
            (unsigned int)script->m_pCurrentIP,
            script->m_bIsMission,
            script->m_bIsExternal,
            script->m_bCondResult,
            script->m_bNotFlag);
    }

    CreateDirectoryA("CoopAndreas_crashes", NULL);

    char filename[260];
    sprintf_s(filename, "CoopAndreas_crashes\\%04d-%02d-%02d_%02d-%02d-%02d.log",
        CCrashLog::ms_exceptionTime.tm_year + 1900, CCrashLog::ms_exceptionTime.tm_mon + 1, CCrashLog::ms_exceptionTime.tm_mday,
        CCrashLog::ms_exceptionTime.tm_hour, CCrashLog::ms_exceptionTime.tm_min, CCrashLog::ms_exceptionTime.tm_sec);

    HANDLE hFile = CreateFileA(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        WriteFile(hFile, ms_szCrashMessage, strlen(ms_szCrashMessage), NULL, NULL);
        CloseHandle(hFile);
        ms_bSuccessSavedLog = true;
    }

    HMODULE instance = GetModuleHandle("CoopAndreasSA.dll");
    if (instance)
    {
        DialogBoxParam(instance, MAKEINTRESOURCE(IDD_CRASHREPORT), 0, DialogProc, 0);
    }

    if (ms_lpPreviousFilter)
    {
        return ms_lpPreviousFilter(exceptionInfo);
    }
    else
    {
        return 0;
    }
}