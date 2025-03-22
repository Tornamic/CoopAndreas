#include "stdafx.h"
#include "CCommandRemoveActorBlipForNetworkPlayer.h"

void CCommandRemoveActorBlipForNetworkPlayer::Process(CRunningScript* script)
{
	script->CollectParameters(2);

	auto networkPlayer = CNetworkPlayerManager::GetPlayer(CPools::GetPed(ScriptParams[0]));
	auto networkVehicle = CNetworkVehicleManager::GetVehicle(CPools::GetVehicle(ScriptParams[1]));

	CPackets::RemoveEntityBlip packet{};
	packet.playerid = networkPlayer->m_iPlayerId;
	packet.entityType = eNetworkEntityType::NETWORK_ENTITY_TYPE_VEHICLE;
	packet.entityId = networkVehicle->m_nVehicleId;
	CNetwork::SendPacket(CPacketsID::REMOVE_ENTITY_BLIP, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
}
