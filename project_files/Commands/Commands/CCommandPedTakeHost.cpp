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
				CPackets::PedTakeHost packet{};
				packet.pedid = networkPed->m_nPedId;
				packet.allowReturnToPreviousHost = ScriptParams[1] != 0;
				CNetwork::SendPacket(CPacketsID::PED_TAKE_HOST, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
			}
		}
	}
}
