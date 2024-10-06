#include "stdafx.h"
static HWND hWnd = nullptr;
RwV2d* lastMousePos = nullptr;

WNDPROC prevWndProc;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CChat::WndProc(hWnd, message, wParam, lParam);
	return CallWindowProc(prevWndProc, hWnd, message, wParam, lParam);
}

void psMouseSetPos(RwV2d* pos)
{
	bool isWindowFocused = GetForegroundWindow() == hWnd;
	if (!isWindowFocused)
		return;

	POINT point = { LONG(pos->x), LONG(pos->y) };
	::ClientToScreen(hWnd, &point);
	::SetCursorPos(point.x, point.y);
	lastMousePos = pos;
}

void InitWndProc()
{
	HWND hWnd = *(HWND*)0xC97C1C;

	SetWindowText(hWnd, "CoopAndreas v0.0.0.0..00000000001111111indev");

	if (hWnd) 
	{
		prevWndProc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
		SetWindowLong(hWnd, GWL_WNDPROC, (LONG)WindowProc);
		return;
	}

	patch::ReplaceFunction(0x7453F0, psMouseSetPos);
	if (hWnd == GetForegroundWindow()) {
		patch::Nop(0x74542B, 8);
	}
	else {
		patch::Nop(0x745433, 8);
	}
}

void CCore::Init()
{
#ifdef _DEV
	CCore::AllocateConsole();
#endif
	CPatch::ApplyPatches();
	CHook::Init();
	CDXFont::Init();
	CLaunchManager::CollectCommandLineArgs();
	Events::initGameEvent.after += []
	{
		// init CNetworking async
		CreateThread(NULL, NULL, CNetwork::InitAsync, NULL, NULL, NULL);
		InitWndProc();
	};
	gameShutdownEvent.before += []
	{
		// disconnect from server
		CNetwork::Disconnect();
	};
}

void CCore::AllocateConsole()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
}
