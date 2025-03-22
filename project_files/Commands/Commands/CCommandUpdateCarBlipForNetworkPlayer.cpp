#include "stdafx.h"
#include "CCommandUpdateCarBlipForNetworkPlayer.h"

void CCommandUpdateCarBlipForNetworkPlayer::Process(CRunningScript* script)
{
	script->CollectParameters(6);

	CPackets::UpdateEntityBlip packet{};
	packet.playerid = CNetworkPlayerManager::GetPlayer(CPools::GetPed(ScriptParams[0]))->m_iPlayerId;
	packet.entityType = eNetworkEntityType::NETWORK_ENTITY_TYPE_VEHICLE;
	packet.entityId = CNetworkVehicleManager::GetVehicle(CPools::GetVehicle(ScriptParams[1]))->m_nVehicleId;
	packet.isFriendly = ScriptParams[2] != 0;
	packet.display = ScriptParams[3];
	packet.color = ScriptParams[4];
	packet.scale = ScriptParams[5];
	//CChat::AddMessage("%d, %d", packet.color, ScriptParams[4]);

	CNetwork::SendPacket(CPacketsID::UPDATE_ENTITY_BLIP, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
}