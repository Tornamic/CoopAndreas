#include "stdafx.h"
#include "CUnicode.h"

std::wstring CUnicode::ConvertWideCharToUtf16Char(wchar_t wideChr)
{
    return std::wstring(1, wideChr);
}

std::wstring CUnicode::ConvertUtf8ToUtf16(const std::string& str)
{
    std::wstring wstr;
    utf8::utf8to16(str.begin(), str.end(), std::back_inserter(wstr));
    return wstr;
}

std::string CUnicode::ConvertUtf16ToUtf8(const std::wstring& wstr)
{
    std::string str;
    utf8::utf16to8(wstr.begin(), wstr.end(), std::back_inserter(str));
    return str;
}