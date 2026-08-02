#pragma once

class CImGui
{
public: 
	static void Init();
	static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static inline bool ms_bActive = false;
};