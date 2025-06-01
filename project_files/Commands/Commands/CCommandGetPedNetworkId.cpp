#include "stdafx.h"
#include "CCommandGetPedNetworkId.h"

void CCommandGetPedNetworkId::Process(CRunningScript* script)
{
	script->CollectParameters(1);

	auto ped = CPools::GetPed(ScriptParams[0]);

	ScriptParams[0] = -1; // the passed ped does not currently have a valid network instance

	if (auto networkPed = CNetworkPedManager::GetPed(ped))
	{
		ScriptParams[0] = networkPed->m_nPedId;
	}

	script->StoreParameters(1);
}
