#include "stdafx.h"
#include "CCommandSetBlipVisibleTo.h"
#include "CRadarSync.h"

void CCommandSetBlipVisibleTo::Process(CRunningScript* script)
{
	script->CollectParameters(2);

	CRadarSync::SetBlipVisibleTo(ScriptParams[0], CPools::GetPed(ScriptParams[1]));
}
