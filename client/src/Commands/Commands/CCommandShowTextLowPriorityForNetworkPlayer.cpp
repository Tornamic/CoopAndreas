#include "stdafx.h"
#include "CCommandShowTextLowPriorityForNetworkPlayer.h"

void CCommandShowTextLowPriorityForNetworkPlayer::Process(CRunningScript* script)
{
	char gxt[9];
	script->ReadTextLabelFromScript(gxt, 8);
	gxt[8] = '\0';

	script->CollectParameters(3);

	auto networkPlayer = CNetworkPlayerManager::GetPlayer(CPools::GetPed(ScriptParams[2]));

	CPacketHandler::AddMessageGXT__Trigger(networkPlayer->m_iPlayerId, gxt, 0, ScriptParams[0], ScriptParams[1]);
}
