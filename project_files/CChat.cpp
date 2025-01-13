#include "stdafx.h"
#include "CUnicode.h"

std::vector<CChatMessage> CChat::m_aMessages{};
std::vector<std::wstring> CChat::m_aPrevMessages{};

std::wstring CChat::m_sInputText = L"";
bool CChat::m_bInputActive = false;
size_t CChat::m_nCaretPos = 0;
uint8_t CChat::m_nCurrentPrevMessageIndex = 0;

uint8_t patch_disable_inputs[] = {0x00, 0x00, 0x00, 0x00, 0x00};

bool IsInputTextEmpty(const std::wstring& str)
{
    return std::all_of(str.begin(), str.end(), [](wchar_t ch) {
        return std::iswspace(ch);
        });
}

void CChat::EraseCharacter(std::wstring& wtext, size_t offCaretPos)
{
    if (m_nCaretPos < 0 || m_nCaretPos + offCaretPos > wtext.size())
        return;

    std::wstring::iterator it = wtext.begin() + m_nCaretPos;

    if (offCaretPos == 0)
    {
        if (it != wtext.begin())
        {
            --it;
            it = wtext.erase(it);
            m_nCaretPos = std::distance(wtext.begin(), it);
        }
    }
    else if (offCaretPos == 1)
    {
        if (it != wtext.end())
        {
            it = wtext.erase(it);
        }
    }
}

void CChat::MoveCaretDirection(bool isMoveRight)
{
    std::wstring::iterator it = m_sInputText.begin() + m_nCaretPos;

    if (isMoveRight)
    {
        if (it != m_sInputText.end())
            ++it;
    }
    else
    {
        if (it != m_sInputText.begin())
            --it;
    }

    m_nCaretPos = std::distance(m_sInputText.begin(), it);
}

void CChat::AddMessage(const std::vector<CTextSegment>& segs)
{
    m_aMessages.push_back(CChatMessage{segs, GetTickCount(), true});
}

void CChat::AddMessage(const std::string& str)
{
    AddMessageRich(CUnicode::ConvertUtf8ToUtf16(str), false);
}

void CChat::AddMessage(const char* format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    std::vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    AddMessageRich(CUnicode::ConvertUtf8ToUtf16(buffer), false);
}

void CChat::AddMessageRich(const std::wstring& str, bool isSplit)
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

void CChat::AddMessage(bool isSplit, const wchar_t* format, ...)
{
    wchar_t buffer[1024];
    va_list args;
    va_start(args, format);
    std::vswprintf(buffer, sizeof(buffer) / sizeof(wchar_t), format, args);
    va_end(args);

    AddMessageRich(std::wstring(buffer), isSplit);
}

void CChat::SendPlayerMessage(const char* name, int id, const wchar_t* message)
{
    CChat::AddMessage(true, L"{FF2D2D}%s(%d): {FFFFFF}%s", CUnicode::ConvertUtf8ToUtf16(name).c_str(), id, message);
}

void CChat::AddPreviousMessage(const std::wstring& message)
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
}

std::vector<std::vector<CTextSegment>> CChat::SplitSegmentsByLength(const std::vector<CTextSegment>& segments)
{
    std::vector<std::vector<CTextSegment>> lines;

    size_t currentLineLen = 0;
    size_t maxLen = MESSAGE_CHUNK_SIZE;

    lines.emplace_back();

    for (const auto& seg : segments)
    {
        size_t pos = 0;
        const std::wstring& segText = seg.text;
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

            size_t take = 0;
            size_t consumed = 0;

            for (size_t i = 0; i < remain && consumed < canFit; ++i)
            {
                wchar_t ch = segText[pos + i];
                take++;
                consumed++;

                if (ch >= 0xD800 && ch <= 0xDBFF && (pos + i + 1) < segText.size())
                {
                    wchar_t nextCh = segText[pos + i + 1];
                    if (nextCh >= 0xDC00 && nextCh <= 0xDFFF)
                    {
                        take++;
                        i++;
                    }
                }
            }

            std::wstring part = segText.substr(pos, take);
            pos += take;

            lines.back().push_back({ part, segColor });

            currentLineLen += consumed;
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
    std::wstring displayText = m_sInputText;

    if (m_nCaretPos <= displayText.size())
    {
        displayText.insert(m_nCaretPos, 1, caretSymbol);
    }
    
    CDXFont::Draw(10, RsGlobal.maximumHeight / 5 + 16 * CDXFont::m_fFontSize, L": " + displayText, D3DCOLOR_RGBA(255, 255, 255, 255));
}

void CChat::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (FrontEndMenuManager.m_bMenuActive)
        return;

    if (message == WM_CHAR && m_bInputActive)
    {
        if (wParam < 32 || wParam == VK_BACK)
            return;

        std::wstring chr = CUnicode::ConvertWideCharToUtf16Char((wchar_t)wParam);

        if (m_sInputText.size() + chr.size() <= MAX_MESSAGE_SIZE)
        {
            m_sInputText.insert(m_nCaretPos, chr);
            m_nCaretPos += chr.size();
        }
    }
    else if (message == WM_KEYDOWN && m_bInputActive)
    {
        if (wParam == VK_LEFT)  // arrow left
        {
            if (m_nCaretPos > 0)
            {
                MoveCaretDirection(false);
            }
        }
        else if (wParam == VK_RIGHT)  // arrow right
        {
            if (m_nCaretPos < m_sInputText.size())
            {
                MoveCaretDirection(true);
            }
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
                m_nCaretPos = m_sInputText.size();
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
                m_nCaretPos = m_sInputText.size();
            }
        }
        else if (wParam == VK_BACK)  // backspace
        {
            EraseCharacter(m_sInputText, 0);
        }
        else if (wParam == VK_DELETE)  // delete
        {
            if (GetKeyState(VK_SHIFT) & 0x8000)
            {
                m_sInputText.clear();
                m_nCaretPos = 0;
                return;
            }

            EraseCharacter(m_sInputText, 1);
        }
        else
        {
            bool isControlV = (GetKeyState(VK_CONTROL) & 0x8000) && wParam == 'V';
            bool isShiftInsert = (GetKeyState(VK_SHIFT) & 0x8000) && wParam == VK_INSERT;

            if (isControlV || isShiftInsert)
            {
                if (!OpenClipboard(nullptr))
                    return;

                HANDLE hData = GetClipboardData(CF_UNICODETEXT);

                CloseClipboard();

                if (!hData)
                    return;

                wchar_t* wideText = (wchar_t*)(GlobalLock(hData));
                
                GlobalUnlock(hData);

                if (!wideText)
                    return;

                std::wstring clipboardText(wideText);

                clipboardText.erase(
                    std::remove_if(clipboardText.begin(), clipboardText.end(), [](wchar_t c) {
                        return (c == L'\r' || c == L'\n');
                        }),
                    clipboardText.end()
                );

                size_t remain = MAX_MESSAGE_SIZE - m_sInputText.size();
                if (clipboardText.size() > remain)
                {
                    clipboardText.resize(remain);
                }

                m_sInputText.insert(m_nCaretPos, clipboardText);
                m_nCaretPos += clipboardText.size();
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
            wcscpy_s(packet.message, m_sInputText.c_str());
            SendPlayerMessage(CLocalPlayer::m_Name, CNetworkPlayerManager::m_nMyId, packet.message);
            AddPreviousMessage(m_sInputText);
            CNetwork::SendPacket(CPacketsID::PLAYER_CHAT_MESSAGE, &packet, sizeof(packet), ENET_PACKET_FLAG_RELIABLE);

            auto it = std::find(m_aPrevMessages.begin(), m_aPrevMessages.end(), m_sInputText);
            if (it != m_aPrevMessages.end())
            {
                m_nCurrentPrevMessageIndex = (uint8_t)(std::distance(m_aPrevMessages.begin(), it)) + 1;
            }

            m_sInputText.clear();
            m_nCaretPos = 0;
        }
    }
}