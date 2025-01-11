#pragma once

#include "CDXFont.h"

#define DEFAULT_CHAT_COLOR(a) D3DCOLOR_RGBA(255, 255, 0, a)

struct CChatMessage
{
	std::vector<CTextSegment> segments;
	DWORD m_nCreatedAt;
	bool m_bVisible;
};

class CChat
{
public:
	static inline constexpr uint8_t MAX_MESSAGES = 15;
	static inline constexpr uint8_t MAX_PREV_MESSAGES = 10;
	static inline constexpr size_t MAX_MESSAGE_AGE = 10000;
	static inline constexpr size_t MESSAGE_DISAPPEAR_TIME = 1000;
	static inline constexpr size_t MESSAGE_CHUNK_SIZE = 64;
	static inline constexpr size_t MAX_MESSAGE_SIZE = 128;
	static inline constexpr size_t CARET_BLINKING_INTERVAL = 500;

	static std::vector<CChatMessage> m_aMessages;
	static std::vector<std::string> m_aPrevMessages;
	static uint8_t m_nCurrentPrevMessageIndex;
	static std::string m_sInputText;
	static bool m_bInputActive;
	static size_t m_nCaretPos;
	static void AddMessage(const std::vector<CTextSegment>& segs);
	static void AddMessage(const std::string& str);
	static void AddMessage(const char* format, ...);
	static void AddMessageRich(const std::string& str, bool bSplit);
	static void AddMessage(bool bSplit, const char* format, ...);
	static void AddPreviousMessage(const std::string& message);
	static std::vector<std::vector<CTextSegment>> SplitSegmentsByLength(const std::vector<CTextSegment>& segments);
	static void SendPlayerMessage(const char* name, int id, const char* message);
	static void Draw();
	static void ToggleInput(bool toggle);
	static void DrawInput();
	static void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};