#include "stdafx.h"
#include "CCommandIsPointOnAnyPlayersScreen.h"
#include "../../Hooks/CCarPedGenHooks.h"

void CCommandIsPointOnAnyPlayersScreen::Process(CRunningScript* script)
{
	script->CollectParameters(4);

	CVector position = *(CVector*)&ScriptParams[0];
	float radius = *(float*)&ScriptParams[3];

	// same as IS_POINT_ON_SCREEN
	if (position.z <= -100.0f)
	{
		position.z = CWorld::FindGroundZForCoord(position.x, position.y);
	}

	script->UpdateCompareFlag(CCarPedGenHooks::IsSphereVisibleForAnyPlayer(&position, radius));
}
