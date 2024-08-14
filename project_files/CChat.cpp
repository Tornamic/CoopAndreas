#include "stdafx.h"

std::vector<std::string> CChat::m_aMessages = 
{ 
    "Welcome to CoopAndreas!",
    "",
    "---------      Authors      --------",
    "",
    "Mod",
    "Tornamic, sr_milton",
    "",
    "Launcher",
    "EOS, Tornamic",
    "",
    "Special thanks",
    "Lone.Rider, The Empty Prod.",
    "and CoopAndreas Discord Community <3",
    "------------------------------------",
    ""
};

void CChat::AddMessage(const std::string& message)
{
    m_aMessages.push_back(message);
}

void CChat::AddMessage(const char* format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    std::vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    AddMessage(std::string(buffer));
}

void CChat::Draw()
{
    if (CDXFont::m_pD3DXFont)
    {
        if (CChat::m_aMessages.size() > 0)
        {
            int i = CChat::m_aMessages.size() - 1;
            int j = 15 - 1;
            while (i >= 0 && j >= 0) 
            {
                CDXFont::Draw(10, RsGlobal.maximumHeight / 3  + j * 15, CChat::m_aMessages[i].c_str(), D3DCOLOR_RGBA(255, 255, 0, 255));
                i--;
                j--;
            }
        }
    }
}