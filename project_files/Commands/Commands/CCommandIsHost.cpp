#include "stdafx.h"
#include "CCommandIsHost.h"

void CCommandIsHost::Process(CRunningScript* script)
{
	script->UpdateCompareFlag(CLocalPlayer::m_bIsHost);
}
