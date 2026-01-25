#include "stdafx.h"
#include "CCommandGetVehicleNetworkId.h"

void CCommandGetVehicleNetworkId::Process(CRunningScript* script)
{
	script->CollectParameters(1);

	auto vehicle = CPools::GetVehicle(ScriptParams[0]);

	ScriptParams[0] = -1; // the passed vehicle does not currently have a valid network instance

	if (auto networkVehicle = CNetworkVehicleManager::GetVehicle(vehicle))
	{
		ScriptParams[0] = networkVehicle->m_nVehicleId;
	}

	script->StoreParameters(1);
}
