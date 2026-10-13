#include "stdafx.h"
#include "CCommandGetNetworkPlayerPedHandleTo.h"

void CCommandGetNetworkPlayerPedHandleTo::Process(CRunningScript* script)
{
	script->CollectParameters(1);

	// SCM integers are 32-bit, the raw bits of a playerid_t
	auto networkPlayer = CNetworkPlayerManager::GetPlayer(playerid_t(static_cast<uint32_t>(ScriptParams[0])));
	//CChat::AddMessage("network player id: %d", ScriptParams[0]);
	ScriptParams[0] = CPools::GetPedRef(networkPlayer->m_pPed);
	//CChat::AddMessage("ped ref: %d", ScriptParams[0]);
	script->StoreParameters(1);
}