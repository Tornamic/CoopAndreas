#pragma once
class CChat
{
public:
	static std::vector<std::string> CChat::m_aMessages;
	static std::string m_sInputText;
	static bool CChat::m_bInputActive;
	static int CChat::m_nCaretPos;
	static void CChat::AddMessage(const std::string& message);
	static void AddMessage(const char* format, ...);
	static void Draw();
	static void ShowInput(bool show);
	static void DrawInput();
	static void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

