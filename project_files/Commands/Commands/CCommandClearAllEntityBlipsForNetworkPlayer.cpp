#include "stdafx.h"
#include "CCommandClearAllEntityBlipsForNetworkPlayer.h"

void CCommandClearAllEntityBlipsForNetworkPlayer::Process(CRunningScript* script)
{
	script->CollectParameters(1);
	auto networkPlayer = CNetworkPlayerManager::GetPlayer(CPools::GetPed(ScriptParams[0]));
	CPackets::ClearEntityBlips packet{};
	packet.playerid = networkPlayer->m_iPlayerId;
	CNetwork::SendPacket(CPacketsID::CLEAR_ENTITY_BLIPS, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
}
