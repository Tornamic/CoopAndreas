#include "stdafx.h"

void CCore::Init()
{
#ifdef _DEV
	CCore::AllocateConsole();
#endif
	CPatch::ApplyPatches();
	CHook::Init();
	CNetwork::Init("127.0.0.1", 6767);

}

void CCore::AllocateConsole()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
}