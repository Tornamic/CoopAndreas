#pragma once

class CImGui
{
public: 
	static void Init();
	static void SetActive(bool bActive);
	static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static inline bool ms_bActive = false;
};
