#include "stdafx.h"
#include "CCommandUpdateCheckpointForNetworkPlayer.h"

void CCommandUpdateCheckpointForNetworkPlayer::Process(CRunningScript* script)
{
	script->CollectParameters(7);
	
	//CChat::AddMessage("CCommandUpdateCheckpointForNetworkPlayer::Process");
	Packets::Blips::UpdateCheckpoint packet{};
	packet.forWhoPlayerId = CNetworkPlayerManager::GetPlayer(CPools::GetPed(ScriptParams[6]))->m_iPlayerId;
	packet.position = *(CVector*)&ScriptParams[0];
	packet.radius = *(CVector*)&ScriptParams[3];
	GetPacketFactory().Send(packet);
}
