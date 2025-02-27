#include "stdafx.h"
#include "CCommandUpdateCheckpointForNetworkPlayer.h"

void CCommandUpdateCheckpointForNetworkPlayer::Process(CRunningScript* script)
{
	script->CollectParameters(7);
	
	//CChat::AddMessage("CCommandUpdateCheckpointForNetworkPlayer::Process");
	CPackets::UpdateCheckpoint packet{};
	packet.playerid = CNetworkPlayerManager::GetPlayer(CPools::GetPed(ScriptParams[6]))->m_iPlayerId;
	packet.position = *(CVector*)&ScriptParams[0];
	packet.radius = *(CVector*)&ScriptParams[3];
	CNetwork::SendPacket(CPacketsID::UPDATE_CHECKPOINT, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
}
