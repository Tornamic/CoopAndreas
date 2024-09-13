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

std::string CChat::m_sInputText = "";
bool CChat::m_bInputActive = false;
int CChat::m_nCaretPos = 0;

unsigned char patch_disable_inputs[] = {0x00, 0x00, 0x00, 0x00, 0x00};

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
                CDXFont::Draw(10, RsGlobal.maximumHeight / 3 + j * RENDER_FONT_SIZE, CChat::m_aMessages[i].c_str(), D3DCOLOR_RGBA(255, 255, 0, 255));
                i--;
                j--;
            }
        }
    }
}

void CChat::ShowInput(bool show)
{
    m_bInputActive = show;

    if (show)
    {
        patch::GetRaw(0x53BEE6, patch_disable_inputs, 5); // save bytes, we cant just nop it without remembering
        patch::Nop(0x53BEE6, 5);
    }
    else
    {
        patch::SetRaw(0x53BEE6, patch_disable_inputs, 5);
    }
    CPad::NewMouseControllerState.x = 0;
    CPad::NewMouseControllerState.y = 0;
    CPad::ClearMouseHistory();
    CPad::GetPad(0)->UpdateMouse();
}

void CChat::DrawInput()
{
    if (!m_bInputActive)
        return;

    std::string caretSymbol = (GetTickCount() % 1000 > 500) ? "|" : " "; // caret blinking

    std::string displayText = m_sInputText;

    if (m_nCaretPos >= 0 && m_nCaretPos <= displayText.length())
    {
        displayText.insert(m_nCaretPos, caretSymbol);
    }

    CDXFont::Draw(10, RsGlobal.maximumHeight / 3 + 15 * RENDER_FONT_SIZE, (": " + displayText).c_str(), D3DCOLOR_RGBA(255, 255, 255, 255));
}

void CChat::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (FrontEndMenuManager.m_bMenuActive)
        return;

    if (message == WM_CHAR && m_bInputActive)
    {
        if (wParam < 32)
            return;

        if (m_sInputText.length() < 128)  // text typing
        {
            m_sInputText.insert(m_nCaretPos, 1, static_cast<char>(wParam));
            m_nCaretPos++;
        }
    }
    else if (message == WM_KEYDOWN && m_bInputActive)
    {
        if (wParam == VK_LEFT)  // arrow left
        {
            if (m_nCaretPos > 0)
                m_nCaretPos--;

        }
        else if (wParam == VK_RIGHT)  // arrow right
        {
            if (m_nCaretPos < m_sInputText.length())
                m_nCaretPos++;
        }

        else if (wParam == VK_BACK)  // backspace
        {
            if (m_nCaretPos > 0)
            {
                m_sInputText.erase(m_nCaretPos - 1, 1);
                m_nCaretPos--;
            }
        }

        else if (wParam == VK_DELETE)  // delete
        {
            if (m_nCaretPos < m_sInputText.length())
                m_sInputText.erase(m_nCaretPos, 1);
        }
    }
    else if (message == WM_KEYUP)
    {
        if (wParam == VK_F6)  // chat toggle
        {
            CChat::ShowInput(!m_bInputActive);
        }
        else if (wParam == VK_ESCAPE && m_bInputActive)
        {
            CChat::ShowInput(false);
        }
        else if (wParam == VK_RETURN && m_bInputActive)
        {
            CChat::ShowInput(false);

            if (!m_sInputText.empty())
            {
                CPackets::PlayerChatMessage packet{};
                strcpy_s(packet.message, m_sInputText.c_str());
                CNetwork::SendPacket(CPacketsID::PLAYER_CHAT_MESSAGE, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
                
                CChat::AddMessage("%s(%d): %s", CLocalPlayer::m_Name, CNetworkPlayerManager::m_nMyId, packet.message);
                
                m_sInputText.clear();
                m_nCaretPos = 0;
            }
        }
    }
}