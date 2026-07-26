#include "stdafx.h"
#include "CCommandPedTakeHost.h"

void CCommandPedTakeHost::Process(CRunningScript* script)
{
	script->CollectParameters(2);

	if (!CLocalPlayer::m_bIsHost)
		return;

	if (auto ped = CPools::GetPed(ScriptParams[0]))
	{
		if (auto networkPed = CNetworkPedManager::GetPed(ped))
		{
			if (!networkPed->m_bSyncing)
			{
				Packets::Peds::PedTakeHost packet{};
				packet.pedid = networkPed->m_nPedId;
				packet.allowReturnToPreviousHost = ScriptParams[1] != 0;
				GetPacketFactory().Send(packet);
			}
		}
	}
}
