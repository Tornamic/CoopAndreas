#include "stdafx.h"



void CCore::Init()
{
#ifdef _DEV
	CCore::AllocateConsole();
#endif
	CPatch::ApplyPatches();
	CHook::Init();
	CDXFont::Init();

	Events::initGameEvent.after += []
	{
		// init CNetworking async
		CreateThread(NULL, NULL, CNetwork::InitAsync, NULL, NULL, NULL);
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