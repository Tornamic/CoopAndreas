#include "stdafx.h"
#include "CCommandRemoveCheckpointForNetworkPlayer.h"

void CCommandRemoveCheckpointForNetworkPlayer::Process(CRunningScript* script)
{
	script->CollectParameters(1);

	CPackets::RemoveCheckpoint packet{};
	packet.playerid = CNetworkPlayerManager::GetPlayer(CPools::GetPed(ScriptParams[0]))->m_iPlayerId;
	CNetwork::SendPacket(CPacketsID::REMOVE_CHECKPOINT, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
}
