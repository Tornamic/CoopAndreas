#include "stdafx.h"
#include "CCrashLog.h"
#include "Commands/CCustomCommandRegistrar.h"
#include "CDiscordRPC.h"

semver_t CCore::Version;

WNDPROC prevWndProc;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CChat::WndProc(hWnd, message, wParam, lParam);
	return CallWindowProc(prevWndProc, hWnd, message, wParam, lParam);
}

void InitWndProc()
{
	HWND hWnd = *(HWND*)0xC97C1C;

	SetWindowText(hWnd, "CoopAndreas");
	RsGlobal.appName = "CoopAndreas";

	if (hWnd) 
	{
		prevWndProc = (WNDPROC)GetWindowLong(hWnd, GWL_WNDPROC);
		SetWindowLong(hWnd, GWL_WNDPROC, (LONG)WindowProc);
		return;
	}
}

void CCore::Init()
{
#ifdef _DEV
	CCore::AllocateConsole();
#endif
	CPatch::ApplyPatches();
	CHook::Init();
	CCustomCommandRegistrar::Register();
	CDXFont::Init();
	CLaunchManager::CollectCommandLineArgs();
	CDiscordRPC::Init();
	//SetUnhandledExceptionFilter(CCrashLog::ExceptionHandler);
	Events::initGameEvent.after += []
	{
		// init CNetworking async
		CreateThread(NULL, NULL, CNetwork::InitAsync, NULL, NULL, NULL);
		InitWndProc();
	};
	Events::initRwEvent += []
	{
		CPatch::PatchFramerate();
	};
	Events::initScriptsEvent.after += []
	{
		CPatch::TemporaryPatches();
	};
	gameShutdownEvent.before += []
	{
		// disconnect from server
		CNetwork::Disconnect();
	};
	semver_parse(COOPANDREAS_VERSION, &CCore::Version);
}

void CCore::AllocateConsole()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
}
