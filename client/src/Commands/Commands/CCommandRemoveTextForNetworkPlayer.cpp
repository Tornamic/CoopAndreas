#include "stdafx.h"
#include "CCommandRemoveTextForNetworkPlayer.h"

void CCommandRemoveTextForNetworkPlayer::Process(CRunningScript* script)
{
	char gxt[9];
	script->ReadTextLabelFromScript(gxt, 8);
	gxt[8] = '\0';

	script->CollectParameters(1);

	auto networkPlayer = CNetworkPlayerManager::GetPlayer(CPools::GetPed(ScriptParams[0]));
	
	CPackets::RemoveMessageGXT packet{};
	strncpy(packet.gxt, gxt, 8);
	packet.playerid = networkPlayer->m_iPlayerId;
	CNetwork::SendPacket(CPacketsID::REMOVE_MESSAGE_GXT, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
}
