#include "CHttp.h"
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

#define HTTPS_AGENT (L"CoopAndreasHTTPS/1.0")

static std::string Request(const wchar_t* method, const wchar_t* host, const wchar_t* path, const std::string& data)
{
    std::string result;

    HINTERNET hSession =
        WinHttpOpen(HTTPS_AGENT, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

    if (!hSession)
    {
        return result;
    }

    WinHttpSetTimeouts(hSession, 3000, 3000, 5000, 5000);

    HINTERNET hConnect = WinHttpConnect(hSession, host, INTERNET_DEFAULT_HTTPS_PORT, 0);

    if (!hConnect)
    {
        WinHttpCloseHandle(hSession);
        return result;
    }

    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect, method, path, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);

    if (!hRequest)
    {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return result;
    }

    LPCWSTR headers = L"Content-Type: text/plain; charset=utf-8\r\n";

    if (WinHttpSendRequest(hRequest, headers, -1, (LPVOID)data.data(), (DWORD)data.size(), (DWORD)data.size(), 0) &&
        WinHttpReceiveResponse(hRequest, NULL))
    {
        DWORD available = 0;

        while (WinHttpQueryDataAvailable(hRequest, &available) && available)
        {
            std::string buffer(available, '\0');

            DWORD read = 0;
            WinHttpReadData(hRequest, buffer.data(), available, &read);

            result.append(buffer.data(), read);
        }
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return result;
}

std::string CHttp::Get(const wchar_t* host, const wchar_t* path)
{
    return Request(L"GET", host, path, "");
}

std::string CHttp::Post(const wchar_t* host, const wchar_t* path, const std::string& data)
{
    return Request(L"POST", host, path, data);
}
