#include "stdafx.h"
#include "CCommandRemoveActorBlipForNetworkPlayer.h"

void CCommandRemoveActorBlipForNetworkPlayer::Process(CRunningScript* script)
{
	script->CollectParameters(2);

	auto networkPlayer = CNetworkPlayerManager::GetPlayer(CPools::GetPed(ScriptParams[0]));
	auto networkPed = CNetworkPedManager::GetPed(CPools::GetPed(ScriptParams[1]));

	CPackets::RemoveEntityBlip packet{};
	packet.playerid = networkPlayer->m_iPlayerId;
	packet.entityType = eNetworkEntityType::NETWORK_ENTITY_TYPE_PED;
	packet.entityId = networkPed->m_nPedId;
	CNetwork::SendPacket(CPacketsID::REMOVE_ENTITY_BLIP, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
}
