#include "stdafx.h"
#include "CNetworkPlayerList.h"

void CNetworkPlayerList::Draw()
{
	std::stringstream ss;
	ss << "Players: " << std::to_string(CNetworkPlayerManager::m_pPlayers.size() + 1) << "\n"; // +1 cuz the local player is not a network player

	if (GetAsyncKeyState(VK_TAB))
	{
		ss << "    " << CLocalPlayer::m_Name << "(" << std::to_string(CNetworkPlayerManager::m_nMyId) << ")\n";

		for (auto networkPlayer : CNetworkPlayerManager::m_pPlayers)
		{
			ss << "    " << networkPlayer->GetName() << "(" << std::to_string(networkPlayer->m_iPlayerId) << ")\n";
		}
	}
	CDXFont::Draw(10, 10, ss.str().c_str(), D3DCOLOR_ARGB(255, 255, 255, 255));
}
