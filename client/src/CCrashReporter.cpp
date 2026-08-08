#include "CCrashReporter.h"
#include "CHttp.h"
#include <thread>

// clang-format off

#define ADDRESS_FETCH_FROM (L"gist.githubusercontent.com")
#define ENDPOINT_FETCH_FROM (L"/Tornamic/a94b7b8dd2ad41c64e2eca5f05490ce7/raw/gistfile1.txt")

#define ENDPOINT_POST_TO (L"/api/logs")

// clang-format on

void CCrashReporter::FetchIP()
{
    if (!ms_bReportingEnabled)
    {
        logger::info("CCrashReporter::FetchIP - ms_bReportingEnabled is FALSE, crash logs won't be sent");
        return;
    }

    if (ms_bReady)
    {
        logger::warn("CCrashReporter::FetchIP - ms_bReady is already TRUE");
        return;
    }

    std::thread(
        []()
        {
            std::string address = CHttp::Get(ADDRESS_FETCH_FROM, ENDPOINT_FETCH_FROM);
            if (!address.empty())
            {
                ms_sAddress = std::move(address);
                ms_bReady = true;
                logger::info("CCrashReporter::FetchIP - Crash reporting server %s", ms_sAddress.c_str());
            }
        })
        .detach();
}

static void _ReplaceAllStr(std::string& str, const std::string& from, const std::string& to)
{
    if (from.empty())
        return;

    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos)
    {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
}

void AnonymizeCrashLog(std::string& content)
{
    char path[MAX_PATH];

    // %GAME%
    if (GetModuleFileNameA(NULL, path, MAX_PATH))
    {
        char* slash = strrchr(path, '\\');
        if (slash)
        {
            *slash = '\0'; // remove "gta_sa.exe"
            _ReplaceAllStr(content, path, "%GAME%");
        }
    }

    // %WINDIR%
    if (GetWindowsDirectoryA(path, MAX_PATH))
    {
        _ReplaceAllStr(content, path, "%WINDIR%");
    }

    // %USER%
    if (GetEnvironmentVariableA("USERPROFILE", path, MAX_PATH))
    {
        _ReplaceAllStr(content, path, "%USER%");
    }
}

void CCrashReporter::PostCrashLog(std::string& content)
{
    if (!ms_bReportingEnabled)
    {
        logger::info("CCrashReporter::PostCrashLog - ms_bReportingEnabled is FALSE, crash logs won't be sent");
        return;
    }

    if (!ms_bReady)
    {
        logger::warn("CCrashReporter::PostCrashLog - ms_bReady is FALSE");
        return;
    }
    
    AnonymizeCrashLog(content);

    ms_postThread = std::thread(
        [content]()
        {
            std::wstring wAddress(ms_sAddress.begin(), ms_sAddress.end());
            CHttp::Post(wAddress.c_str(), ENDPOINT_POST_TO, content);
        });
}
