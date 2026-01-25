#include "stdafx.h"
#include "CCommandGetNetworkPlayerInternalId.h"

void CCommandGetNetworkPlayerInternalId::Process(CRunningScript* script)
{
	script->CollectParameters(1);

	int pedHandle = ScriptParams[0];

	ScriptParams[0] = -1;

	if (auto ped = CPools::GetPed(pedHandle))
	{
		if (auto networkPlayer = CNetworkPlayerManager::GetPlayer(ped))
		{
			ScriptParams[0] = networkPlayer->GetInternalId();
		}
	}

	script->StoreParameters(1);
}
