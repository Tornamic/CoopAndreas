#include "stdafx.h"
#include "CCommandShowTextStyledForNetworkPlayer.h"

void CCommandShowTextStyledForNetworkPlayer::Process(CRunningScript* script)
{
	char gxt[8];
	script->ReadTextLabelFromScript(gxt, 8);
	gxt[7] = '\0';

	script->CollectParameters(3);

	auto networkPlayer = CNetworkPlayerManager::GetPlayer(CPools::GetPed(ScriptParams[2]));

	Packets::Scripts::AddMessageGXT packet{};
	strncpy(packet.gxt, gxt, 8);
	packet.forWhoPlayerId = networkPlayer->m_iPlayerId;
	packet.type = Packets::Scripts::AddMessageGXT::eGXTMsgType::sync_COMMAND_PRINT_BIG;
	packet.time = ScriptParams[0];
	packet.flag = ScriptParams[1];
	GetPacketFactory().Send(packet);
}
