#include "stdafx.h"

std::vector<CNetworkPlayer*> CNetworkPlayerManager::m_pPlayers;
CPad CNetworkPlayerManager::m_pPads[MAX_SERVER_PLAYERS + 2];

void CNetworkPlayerManager::Add(CNetworkPlayer* player)
{
	m_pPlayers.push_back(player);
}

void CNetworkPlayerManager::Remove(CNetworkPlayer* player)
{
	auto it = std::find(m_pPlayers.begin(), m_pPlayers.end(), player);
	if (it != m_pPlayers.end())
	{
		m_pPlayers.erase(it);
	}
}

CNetworkPlayer* CNetworkPlayerManager::GetPlayer(CPlayerPed* playerPed)
{
	for (int i = 0; i != m_pPlayers.size(); i++)
	{
		if (m_pPlayers[i]->m_pPed == playerPed)
		{
			return m_pPlayers[i];
		}
	}
	return nullptr;
}

CNetworkPlayer* CNetworkPlayerManager::GetPlayer(CPed* ped)
{
	for (int i = 0; i != m_pPlayers.size(); i++)
	{
		if (m_pPlayers[i]->m_pPed == ped)
		{
			return m_pPlayers[i];
		}
	}
	return nullptr;
}

CNetworkPlayer* CNetworkPlayerManager::GetPlayer(int playerid)
{
	for (int i = 0; i != m_pPlayers.size(); i++)
	{
		if (m_pPlayers[i]->m_iPlayerId == playerid)
		{
			return m_pPlayers[i];
		}
	}
	return nullptr;
}
