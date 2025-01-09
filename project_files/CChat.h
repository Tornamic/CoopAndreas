#pragma once

#define DEFAULT_CHAT_COLOR(a) D3DCOLOR_RGBA(255, 255, 0, a)

struct CChatMessage
{
	std::string m_message;
	DWORD m_nCreatedAt;
	bool m_bVisible;
};

class CChat
{
public:
	static inline constexpr uint8_t CChat::MAX_MESSAGES = 15;
	static inline constexpr uint8_t CChat::MAX_PREV_MESSAGES = 10;
	static inline constexpr size_t CChat::MAX_MESSAGE_AGE = 10000;
	static inline constexpr size_t CChat::MESSAGE_DISAPPEAR_TIME = 1000;
	static inline constexpr size_t CChat::MESSAGE_CHUNK_SIZE = 64;
	static inline constexpr size_t CChat::MAX_MESSAGE_SIZE = 128;

	static std::vector<CChatMessage> CChat::m_aMessages;
	static std::vector<std::string> CChat::m_aPrevMessages;
	static uint8_t m_iCurrentPrevMessageIndex;
	static std::string m_sInputText;
	static bool CChat::m_bInputActive;
	static size_t CChat::m_nCaretPos;
	static void CChat::AddMessage(const std::string& message);
	static void AddMessage(const char* format, ...);
	static void AddPreviousMessage(const std::string& message);
	static std::vector<std::string> SeparateMessageToChunks(const std::string& message);
	static void SendPlayerMessage(const char* name, int id, const char* message);
	static void Draw();
	static void ToggleInput(bool toggle);
	static void DrawInput();
	static void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};