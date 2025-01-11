#include "stdafx.h"

std::vector<CChatMessage> CChat::m_aMessages{};
std::vector<std::string> CChat::m_aPrevMessages{};

std::string CChat::m_sInputText = "";
bool CChat::m_bInputActive = false;
size_t CChat::m_nCaretPos = 0;
uint8_t CChat::m_nCurrentPrevMessageIndex = 0;

uint8_t patch_disable_inputs[] = {0x00, 0x00, 0x00, 0x00, 0x00};

bool IsInputTextEmpty(const std::string& str)
{
    return str.find_first_not_of("\t\n ") == std::string::npos;
}

void CChat::AddMessage(const std::vector<CTextSegment>& segs)
{
    m_aMessages.push_back(CChatMessage{segs, GetTickCount(), true});
}

void CChat::AddMessage(const std::string& str)
{
    AddMessageRich(str, false);
}

void CChat::AddMessage(const char* format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    std::vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    AddMessageRich(std::string(buffer), false);
}

void CChat::AddMessageRich(const std::string& str, bool isSplit)
{
    std::vector<CTextSegment> segments = CDXFont::ParseColorSegments(str, DEFAULT_CHAT_COLOR(255));

    if (segments.empty())
        return;

    if (isSplit)
    {
        std::vector<std::vector<CTextSegment>> lines = SplitSegmentsByLength(segments);
        for (auto& line : lines)
        {
            if (!line.empty())
                AddMessage(line);
        }
    }
    else
    {
        AddMessage(segments);
    }
}

void CChat::AddMessage(bool isSplit, const char* format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    std::vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    AddMessageRich(std::string(buffer), isSplit);
}

void CChat::SendPlayerMessage(const char* name, int id, const char* message)
{
    CChat::AddMessage(true, "{FF2D2D}%s(%d): {FFFFFF}%s", name, id, message);
}

void CChat::AddPreviousMessage(const std::string& message)
{
    if (std::find(m_aPrevMessages.begin(), m_aPrevMessages.end(), message) != m_aPrevMessages.end())
    {
        return;
    }

    if ((uint8_t)m_aPrevMessages.size() >= CChat::MAX_PREV_MESSAGES)
    {
        m_aPrevMessages.erase(m_aPrevMessages.begin());

        if (m_nCurrentPrevMessageIndex > 0)
        {
            m_nCurrentPrevMessageIndex--;
        }
    }

    m_aPrevMessages.push_back(message);

    if ((uint8_t)m_aPrevMessages.size() < CChat::MAX_PREV_MESSAGES)
    {
        m_nCurrentPrevMessageIndex = (uint8_t)m_aPrevMessages.size();
    }
}

std::vector<std::vector<CTextSegment>> CChat::SplitSegmentsByLength(const std::vector<CTextSegment>& segments)
{
    std::vector<std::vector<CTextSegment>> lines;
   
    size_t currentLineLen = 0;
    size_t maxLen = MESSAGE_CHUNK_SIZE;

    lines.emplace_back();

    for (auto& seg : segments)
    {
        size_t pos = 0;
        const std::string& segText = seg.text;
        D3DCOLOR segColor = seg.color;

        while (pos < segText.size())
        {
            size_t canFit = (maxLen > currentLineLen) ? (maxLen - currentLineLen) : 0;

            if (canFit == 0)
            {
                lines.emplace_back();
                currentLineLen = 0;
                canFit = maxLen;
            }

            size_t remain = segText.size() - pos;
            size_t take = min(canFit, remain);

            if (take == 0)
            {
                break;
            }

            std::string part = segText.substr(pos, take);
            pos += take;

            lines.back().push_back({ part, segColor });

            currentLineLen += take;
        }
    }

    return lines;
}

void CChat::Draw()
{
    if (!CDXFont::m_pD3DXFont || m_aMessages.empty())
        return;

    int tickCount = GetTickCount();
    uint8_t drawnMessages = 0;

    for (auto it = m_aMessages.rbegin(); it != m_aMessages.rend() && drawnMessages < MAX_MESSAGES; ++it)
    {
        if (!it->m_bVisible && !m_bInputActive)
            continue;

        size_t fadeAlpha = 255;
        if (!m_bInputActive)
        {
            DWORD messageAge = tickCount - it->m_nCreatedAt;
            if (messageAge > MAX_MESSAGE_AGE)
            {
                size_t fadeTime = messageAge - MAX_MESSAGE_AGE;
                if (fadeTime <= MESSAGE_DISAPPEAR_TIME)
                {
                    fadeAlpha = 255 - ((fadeTime * 255) / MESSAGE_DISAPPEAR_TIME);
                }
                else
                {
                    it->m_bVisible = false;
                    continue;
                }
            }
        }

        int x = 10;
        int y = RsGlobal.maximumHeight / 5 + (MAX_MESSAGES - drawnMessages) * CDXFont::m_fFontSize;

        for (auto& seg : it->segments)
        {
            uint8_t r = (seg.color >> 16) & 0xFF;
            uint8_t g = (seg.color >> 8) & 0xFF;
            uint8_t b = seg.color & 0xFF;
            uint8_t a = (seg.color >> 24) & 0xFF;

            uint8_t blendedAlpha = (uint8_t)((a * fadeAlpha) / 255);
            D3DCOLOR blendedColor = r << 16 | g << 8 | b & 0xFF | blendedAlpha << 24;

            CDXFont::DrawSegmentWithShadow(x, y, seg.text.c_str(), blendedColor);
            x += CDXFont::GetTextWidth(seg.text);
        }

        drawnMessages++;
    }

    if (m_aMessages.size() > CChat::MAX_MESSAGES)
    {
        m_aMessages.erase(m_aMessages.begin());
    }
}

void CChat::ToggleInput(bool toggle)
{
    m_bInputActive = toggle;

    if (toggle)
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

    char caretSymbol = (GetTickCount() % 1000 > CARET_BLINKING_INTERVAL) ? '|' : ' '; // caret blinking
    std::string displayText = m_sInputText;

    if (m_nCaretPos >= 0 && m_nCaretPos <= displayText.length())
    {
        displayText.insert(m_nCaretPos, 1, caretSymbol);
    }
    
    CDXFont::Draw(10, RsGlobal.maximumHeight / 5 + 16 * CDXFont::m_fFontSize, (": " + displayText).c_str(), D3DCOLOR_RGBA(255, 255, 255, 255));
}

void CChat::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (FrontEndMenuManager.m_bMenuActive)
        return;

    if (message == WM_CHAR && m_bInputActive)
    {
        if (wParam < 32)
            return;

        if (m_sInputText.length() < CChat::MAX_MESSAGE_SIZE)  // text typing
        {
            m_sInputText.insert(m_nCaretPos, 1, (char)(wParam));
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
        else if (wParam == VK_UP)
        {
            if (!m_sInputText.empty())
            {
                AddPreviousMessage(m_sInputText);
            }

            if (!m_aPrevMessages.empty())
            {
                if (m_nCurrentPrevMessageIndex > 0)
                {
                    m_nCurrentPrevMessageIndex--;
                }

                m_sInputText = m_aPrevMessages[m_nCurrentPrevMessageIndex];
                m_nCaretPos = m_sInputText.length();
            }
        }
        else if (wParam == VK_DOWN)
        {
            if (!m_sInputText.empty())
            {
                AddPreviousMessage(m_sInputText);
            }

            if (!m_aPrevMessages.empty())
            {
                m_nCurrentPrevMessageIndex++;

                if (m_nCurrentPrevMessageIndex >= (uint8_t)m_aPrevMessages.size())
                {
                    m_nCaretPos = 0;
                    m_sInputText.clear();
                    m_nCurrentPrevMessageIndex = (uint8_t)m_aPrevMessages.size();
                    return;
                }
               
                m_sInputText = m_aPrevMessages[m_nCurrentPrevMessageIndex];
                m_nCaretPos = m_sInputText.length();
            }
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
            if (GetKeyState(VK_SHIFT) & 0x8000)
            {
                m_sInputText.clear();
                m_nCaretPos = 0;
            }
            else if (m_nCaretPos < m_sInputText.length())
                m_sInputText.erase(m_nCaretPos, 1);
        }
        else
        {
            if (message == WM_PASTE)
            {
                if (!OpenClipboard(nullptr))
                    return;

                HANDLE hData = GetClipboardData(CF_TEXT);
                if (!hData)
                {
                    CloseClipboard();
                    return;
                }

                char* pszText = (char*)(GlobalLock(hData));
                if (!pszText)
                {
                    CloseClipboard();
                    return;
                }

                std::string clipboardText(pszText);
                GlobalUnlock(hData);

                size_t remain = CChat::MAX_MESSAGE_SIZE - m_sInputText.length();
                if (clipboardText.size() > remain)
                {
                    clipboardText.resize(remain);
                }

                m_sInputText.insert(m_nCaretPos, clipboardText);
                m_nCaretPos += clipboardText.size();

                CloseClipboard();
            }
        }
    }
    else if (message == WM_KEYUP)
    {
        if (wParam == VK_F6)  // chat toggle
        {
            ToggleInput(!m_bInputActive);
        }
        else if (wParam == VK_ESCAPE && m_bInputActive)
        {
            ToggleInput(false);
        }
        else if (wParam == VK_RETURN && m_bInputActive)
        {
            if (!CNetwork::m_bConnected)
                return;

            ToggleInput(false);

            if (m_sInputText.empty())
                return;

            if (IsInputTextEmpty(m_sInputText))
            {
                m_sInputText.clear();
                m_nCaretPos = 0;
                return;
            }

            CPackets::PlayerChatMessage packet{};
            strcpy_s(packet.message, m_sInputText.c_str());
            SendPlayerMessage(CLocalPlayer::m_Name, CNetworkPlayerManager::m_nMyId, packet.message);
            AddPreviousMessage(m_sInputText);
            CNetwork::SendPacket(CPacketsID::PLAYER_CHAT_MESSAGE, &packet, sizeof(packet), ENET_PACKET_FLAG_RELIABLE);

            m_sInputText.clear();
            m_nCaretPos = 0;
        }
    }
}