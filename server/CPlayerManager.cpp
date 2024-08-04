#include "stdafx.h"

std::vector<CPlayer*> CPlayerManager::m_pPlayers;

void CPlayerManager::Add(CPlayer* player)
{
	m_pPlayers.push_back(player);
}

void CPlayerManager::Remove(CPlayer* player)
{
	auto it = std::find(m_pPlayers.begin(), m_pPlayers.end(), player);
	if (it != m_pPlayers.end())
	{
		m_pPlayers.erase(it);
	}
}

// find player instance by id
CPlayer* CPlayerManager::GetPlayer(int playerid)
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

// find player instance by enetpeer
CPlayer* CPlayerManager::GetPlayer(ENetPeer* peer)
{
	for (int i = 0; i != m_pPlayers.size(); i++)
	{
		if (m_pPlayers[i]->m_pPeer == peer)
		{
			return m_pPlayers[i];
		}
	}
	return nullptr;
}

int CPlayerManager::GetFreeID()
{
	for (int i = 0; i != MAX_SERVER_PLAYERS; i++)
	{
		if(CPlayerManager::GetPlayer(i) == nullptr)
			return i; 
	}
	return -1; // server is full
}

CPlayer* CPlayerManager::GetHost()
{
	for (int i = 0; i != m_pPlayers.size(); i++)
	{
		if (m_pPlayers[i]->m_bIsHost)
		{
			return m_pPlayers[i];
		}
	}
	return nullptr;
}