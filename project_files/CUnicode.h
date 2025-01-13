#pragma once

class CUnicode
{
public:
	static std::wstring ConvertWideCharToUtf16Char(wchar_t wideChr);
	static std::wstring ConvertUtf8ToUtf16(const std::string& str);
	static std::string ConvertUtf16ToUtf8(const std::wstring& str);
};