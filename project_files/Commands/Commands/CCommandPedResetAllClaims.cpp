#include "stdafx.h"
#include "CCommandPedResetAllClaims.h"

void CCommandPedResetAllClaims::Process(CRunningScript* script)
{
	script->CollectParameters(1);

	if (auto ped = CPools::GetPed(ScriptParams[0]))
	{
		if (auto networkPed = CNetworkPedManager::GetPed(ped))
		{
			if (networkPed->m_bSyncing || CLocalPlayer::m_bIsHost)
			{
				CPackets::PedResetAllClaims packet{};
				packet.pedid = networkPed->m_nPedId;
				CNetwork::SendPacket(CPacketsID::PED_RESET_ALL_CLAIMS, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
			}
		}
	}
}
