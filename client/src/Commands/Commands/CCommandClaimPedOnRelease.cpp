#include "stdafx.h"
#include "CCommandClaimPedOnRelease.h"

void CCommandClaimPedOnRelease::Process(CRunningScript* script)
{
	script->CollectParameters(1);

	if (auto ped = CPools::GetPed(ScriptParams[0]))
	{
		if (auto networkPed = CNetworkPedManager::GetPed(ped))
		{
			networkPed->ClaimOnRelease();
		}
	}
}
