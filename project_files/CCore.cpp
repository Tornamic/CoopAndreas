#include "stdafx.h"

void CCore::Init()
{
	CPatch::ApplyPatches();
	CNetwork::Init("127.0.0.1", 6767);
#ifdef _DEV
	CCore::AllocateConsole();
#endif

}

void CCore::AllocateConsole()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONIN$", "r", stdin);
}