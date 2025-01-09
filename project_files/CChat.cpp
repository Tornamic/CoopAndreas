#include "stdafx.h"
#include "CDXFont.h"

std::vector<CChatMessage> CChat::m_aMessages{};
std::vector<std::string> CChat::m_aPrevMessages{};

std::string CChat::m_sInputText = "";
bool CChat::m_bInputActive = false;
size_t CChat::m_nCaretPos = 0;
uint8_t CChat::m_iCurrentPrevMessageIndex = 0;

uint8_t patch_disable_inputs[] = {0x00, 0x00, 0x00, 0x00, 0x00};

void CChat::AddMessage(const std::string& message)
{
    m_aMessages.push_back(CChatMessage{message, GetTickCount(), true});
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

void CChat::AddPreviousMessage(const std::string& message)
{
    if (std::find(m_aPrevMessages.begin(), m_aPrevMessages.end(), message) != m_aPrevMessages.end())
    {
        return;
    }

    if ((uint8_t)m_aPrevMessages.size() >= CChat::MAX_PREV_MESSAGES)
    {
        m_aPrevMessages.erase(m_aPrevMessages.begin());

        if (m_iCurrentPrevMessageIndex > 0)
        {
            m_iCurrentPrevMessageIndex--;
        }
    }

    m_aPrevMessages.push_back(message);

    if ((uint8_t)m_aPrevMessages.size() < CChat::MAX_PREV_MESSAGES)
    {
        m_iCurrentPrevMessageIndex = (uint8_t)m_aPrevMessages.size();
    }
}

std::vector<std::string> CChat::SeparateMessageToChunks(const std::string& message)
{
    size_t start = 0;
    uint8_t chunkCount = 0;
    size_t length = message.length();
    std::vector<std::string> chunks{};

    while (start < length)
    {
        size_t chunkSize = min(CChat::MESSAGE_CHUNK_SIZE, length - start);
        if (chunkSize == CChat::MESSAGE_CHUNK_SIZE)
        {
            size_t lastSpace = message.rfind(' ', start + chunkSize);
            if (lastSpace != std::string::npos && lastSpace > start)
            {
                chunkSize = lastSpace - start;
            }
        }

        std::string messagePart = message.substr(start, chunkSize);
        chunks.push_back(messagePart);

        start += chunkSize;
        while (start < length && message[start] == ' ')
        {
            start++;
        }

        chunkCount++;
    }

    return chunks;
}

void CChat::SendPlayerMessage(const char* name, int id, const char* message)
{
    std::vector<std::string> chunks = CChat::SeparateMessageToChunks(message);

    for (size_t i = 0; i < chunks.size(); i++)
    {
        if (i == 0)
        {
            CChat::AddMessage("{FF2D2D}%s(%d): {FFFFFF}%s", name, id, chunks[i].c_str());
            continue;
        }

        CChat::AddMessage("{FFFFFF}%s", chunks[i].c_str());
    }
}

void CChat::Draw()
{
    if (!CDXFont::m_pD3DXFont || CChat::m_aMessages.empty())
        return;

    int tickCount = GetTickCount();
    uint8_t drawnMessages = 0;

    for (auto it = CChat::m_aMessages.rbegin(); it != CChat::m_aMessages.rend() && drawnMessages < CChat::MAX_MESSAGES; ++it)
    {
		if (!it->m_bVisible && !m_bInputActive)
		{
			continue;
		}

        size_t alpha = 255;

        if (!m_bInputActive)
        {
            DWORD messageAge = tickCount - it->m_nCreatedAt;

            if (messageAge > CChat::MAX_MESSAGE_AGE)
            {
                size_t fadeTime = messageAge - CChat::MAX_MESSAGE_AGE;
                if (fadeTime <= CChat::MESSAGE_DISAPPEAR_TIME)
                {
                    alpha = 255 - ((fadeTime * 255) / CChat::MESSAGE_DISAPPEAR_TIME);
                }
                else
                {
                    it->m_bVisible = false;
                    continue;
                }
            }
        }

        CDXFont::Draw(10, RsGlobal.maximumHeight / 5 + (CChat::MAX_MESSAGES - drawnMessages) * CDXFont::m_fFontSize, it->m_message.c_str(), DEFAULT_CHAT_COLOR(alpha));
        drawnMessages++;
    }

    if (m_aMessages.size() > CChat::MAX_MESSAGES)
    {
        CChat::m_aMessages.erase(CChat::m_aMessages.begin());
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

    const std::string caretSymbol = (GetTickCount() % 1000 > 500) ? "|" : " "; // caret blinking
    std::string displayText = m_sInputText;

    if (m_nCaretPos >= 0 && m_nCaretPos <= displayText.length())
    {
        displayText.insert(m_nCaretPos, caretSymbol);
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
                CChat::AddPreviousMessage(m_sInputText);
            }

            if (!m_aPrevMessages.empty())
            {
                if (m_iCurrentPrevMessageIndex > 0)
                {
                    m_iCurrentPrevMessageIndex--;
                }

                m_sInputText = m_aPrevMessages[m_iCurrentPrevMessageIndex];
                m_nCaretPos = m_sInputText.length();
            }
        }
        else if (wParam == VK_DOWN)
        {
            if (!m_sInputText.empty())
            {
                CChat::AddPreviousMessage(m_sInputText);
            }

            if (!m_aPrevMessages.empty())
            {
                m_iCurrentPrevMessageIndex++;

                if (m_iCurrentPrevMessageIndex >= (uint8_t)m_aPrevMessages.size())
                {
                    m_nCaretPos = 0;
                    m_sInputText = "";
                    m_iCurrentPrevMessageIndex = (uint8_t)m_aPrevMessages.size();
                    return;
                }
               
                m_sInputText = m_aPrevMessages[m_iCurrentPrevMessageIndex];
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
            if (!CNetwork::m_bConnected)
                return;

            CChat::ToggleInput(!m_bInputActive);
        }
        else if (wParam == VK_ESCAPE && m_bInputActive)
        {
            CChat::ToggleInput(false);
        }
        else if (wParam == VK_RETURN && m_bInputActive)
        {
            CChat::ToggleInput(false);

            if (m_sInputText.empty())
            {
                return;
            }

            CPackets::PlayerChatMessage packet{};
            strcpy_s(packet.message, m_sInputText.c_str());
            CChat::SendPlayerMessage(CLocalPlayer::m_Name, CNetworkPlayerManager::m_nMyId, packet.message);
            CChat::AddPreviousMessage(m_sInputText);
            CNetwork::SendPacket(CPacketsID::PLAYER_CHAT_MESSAGE, &packet, sizeof(packet), ENET_PACKET_FLAG_RELIABLE);

            m_sInputText.clear();
            m_nCaretPos = 0;
        }
    }
}