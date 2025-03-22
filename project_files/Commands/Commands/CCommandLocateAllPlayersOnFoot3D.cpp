#include "stdafx.h"
#include "CCommandLocateAllPlayersOnFoot3D.h"

void CCommandLocateAllPlayersOnFoot3D::Process(CRunningScript* script)
{
	script->CollectParameters(7);

	bool showMarker = ScriptParams[0] != 0;
	CVector* position = (CVector*)&ScriptParams[1];
	CVector* radius = (CVector*)&ScriptParams[4];

	bool result = Command<Commands::LOCATE_CHAR_ON_FOOT_3D>(
		CPools::GetPedRef(FindPlayerPed(0)), 
		position->x, position->y, position->z, 
		radius->x, radius->y, radius->z, 
		showMarker);

	if (result)
	{
		for (auto networkPlayer : CNetworkPlayerManager::m_pPlayers)
		{
			if (networkPlayer->m_pPed)
			{
				result = Command<Commands::LOCATE_CHAR_ON_FOOT_3D>(
					CPools::GetPedRef(networkPlayer->m_pPed), 
					position->x, position->y, position->z, 
					radius->x, radius->y, radius->z, 
					false);
			
				if (!result)
				{
					break;
				}
			}
		}
	}

	script->UpdateCompareFlag(result);
}
