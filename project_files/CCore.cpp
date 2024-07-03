#include "stdafx.h"

void CCore::Init()
{
#ifdef _DEV
	CCore::AllocateConsole();
#endif
	CPatch::ApplyPatches();
	CHook::Init();

	// init CNetworking async
	CreateThread(NULL, NULL, CNetwork::InitAsync, NULL, NULL, NULL);

}

void CCore::AllocateConsole()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
}