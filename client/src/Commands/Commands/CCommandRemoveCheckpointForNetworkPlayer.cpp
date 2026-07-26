#include "stdafx.h"
#include "CCommandRemoveCheckpointForNetworkPlayer.h"

void CCommandRemoveCheckpointForNetworkPlayer::Process(CRunningScript* script)
{
	script->CollectParameters(1);

	Packets::Blips::RemoveCheckpoint packet{};
	packet.forWhoPlayerId = CNetworkPlayerManager::GetPlayer(CPools::GetPed(ScriptParams[0]))->m_iPlayerId;
	GetPacketFactory().Send(packet);
}
