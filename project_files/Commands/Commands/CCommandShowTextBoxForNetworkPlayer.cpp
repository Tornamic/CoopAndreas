#include "stdafx.h"
#include "CCommandShowTextBoxForNetworkPlayer.h"

void CCommandShowTextBoxForNetworkPlayer::Process(CRunningScript* script)
{
	char gxt[9];
	script->ReadTextLabelFromScript(gxt, 8);
	gxt[8] = '\0';

	script->CollectParameters(1);

	auto networkPlayer = CNetworkPlayerManager::GetPlayer(CPools::GetPed(ScriptParams[0]));

	CPacketHandler::AddMessageGXT__Trigger(networkPlayer->m_iPlayerId, gxt, 3);
}
