#include "stdafx.h"


void CPlayerPed__ProcessControl_Hook(CPlayerPed* player)
{
	//
}

void CHook::Init()
{
	//patch::SetPointer(0x86D190, CPlayerPed__ProcessControl_Hook);
}