#include "stdafx.h"
#include "CCommandUpdateCarBlipForNetworkPlayer.h"

void CCommandUpdateCarBlipForNetworkPlayer::Process(CRunningScript* script)
{
	script->CollectParameters(6);

	Packets::Blips::UpdateEntityBlip packet{};
	packet.forWhoPlayerId = CNetworkPlayerManager::GetPlayer(CPools::GetPed(ScriptParams[0]))->m_iPlayerId;
	packet.entity.SetEntity(CPools::GetVehicle(ScriptParams[1]));
	packet.isFriendly = ScriptParams[2] != 0;
	packet.display = static_cast<eBlipDisplay>(ScriptParams[3]);
	packet.color = static_cast<eBlipColour>(ScriptParams[4]);
	packet.scale = ScriptParams[5];
	//CChat::AddMessage("%d, %d", packet.color, ScriptParams[4]);

	GetPacketFactory().Send(packet);
}