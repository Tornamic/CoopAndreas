#include "stdafx.h"

std::vector<CNetworkPlayer*> CNetworkPlayerManager::m_pPlayers;

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

// find player instance by id
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

// find player instance by enetpeer
CNetworkPlayer* CNetworkPlayerManager::GetPlayer(ENetPeer* peer)
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

int CNetworkPlayerManager::GetFreeID()
{
	for (int i = 0; i != MAX_SERVER_PLAYERS; i++)
	{
		if(CNetworkPlayerManager::GetPlayer(i) == nullptr)
			return i; 
	}
	return -1; // server is full
}

CNetworkPlayer* CNetworkPlayerManager::GetHost()
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

void CNetworkPlayerManager::AssignHostToFirstPlayer()
{
	if (CNetworkPlayerManager::m_pPlayers.size() <= 0)
		return;

	CNetworkPlayer* player = CNetworkPlayerManager::m_pPlayers.front();
	CNetworkPlayer* host = CNetworkPlayerManager::GetHost();

	if (player == host)
		return;

	if (host != nullptr)
		host->m_bIsHost = false;

	player->m_bIsHost = true;

	CPackets::PlayerSetHost setHostPacket = { player->m_iPlayerId };
	CNetwork::SendPacketToAll(ePacketType::PLAYER_SET_HOST, &setHostPacket, sizeof setHostPacket, ENET_PACKET_FLAG_RELIABLE, nullptr);
}