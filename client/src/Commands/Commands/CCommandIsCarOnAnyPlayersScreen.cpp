#include "stdafx.h"
#include "CCommandIsCarOnAnyPlayersScreen.h"
#include "../../Hooks/CCarPedGenHooks.h"

void CCommandIsCarOnAnyPlayersScreen::Process(CRunningScript* script)
{
	script->CollectParameters(1);

	if (auto vehicle = CPools::GetVehicle(ScriptParams[0]))
	{
		script->UpdateCompareFlag(CCarPedGenHooks::IsEntityOnAnyPlayersScreen(vehicle));
		return;
	}
	script->UpdateCompareFlag(false);
}
