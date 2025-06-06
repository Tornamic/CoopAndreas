#include "stdafx.h"
#include "CCommandIsSyncingThisPed.h"

void CCommandIsSyncingThisPed::Process(CRunningScript* script)
{
	script->CollectParameters(1);

	if (auto ped = CPools::GetPed(ScriptParams[0]))
	{
		if (auto networkPed = CNetworkPedManager::GetPed(ped))
		{
			script->UpdateCompareFlag(true);
			return;
		}
	}
	script->UpdateCompareFlag(false);
}
