#pragma once
class CCustomMenuManager
{
public:
	static void UpdateFromConfig();
	static void Init();
	static void WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

