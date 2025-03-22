#include "stdafx.h"
#include "CCommandGetNetworkPlayerPedHandleTo.h"

void CCommandGetNetworkPlayerPedHandleTo::Process(CRunningScript* script)
{
	script->CollectParameters(1);

	auto networkPlayer = CNetworkPlayerManager::GetPlayer(ScriptParams[0]);
	//CChat::AddMessage("network player id: %d", ScriptParams[0]);
	ScriptParams[0] = CPools::GetPedRef(networkPlayer->m_pPed);
	//CChat::AddMessage("ped ref: %d", ScriptParams[0]);
	script->StoreParameters(1);
}