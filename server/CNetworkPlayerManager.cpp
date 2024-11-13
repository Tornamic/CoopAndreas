#include "stdafx.h"

#include "CNetworkPlayerManager.h"
#include "CPackets.h"
#include "CNetwork.h"

CNetworkPlayer* CNetworkPlayerManager::Get(ENetPeer* peer)
{
	auto& players = this->GetEntities();

	for (int i = 0; i != players.size(); i++)
	{
		if (players[i]->m_pPeer == peer)
		{
			return players[i];
		}
	}

	return nullptr;
}

CNetworkPlayer* CNetworkPlayerManager::GetHost()
{
	auto& players = this->GetEntities();

	for (int i = 0; i != players.size(); i++)
	{
		if (players[i]->m_bIsHost)
		{
			return players[i];
		}
	}
	return nullptr;
}

void CNetworkPlayerManager::AssignHostToFirstPlayer()
{
	auto& players = this->GetEntities();

	if (players.size() <= 0)
		return;

	CNetworkPlayer* player = players.front();
	CNetworkPlayer* host = this->GetHost();

	if (player == host)
		return;

	if (host != nullptr)
		host->m_bIsHost = false;

	player->m_bIsHost = true;

	CPackets::PlayerSetHost setHostPacket = { player->GetId() };
	CNetwork::SendPacketToAll(ePacketType::PLAYER_SET_HOST, &setHostPacket, sizeof setHostPacket, ENET_PACKET_FLAG_RELIABLE, nullptr);
}