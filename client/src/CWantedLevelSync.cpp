#include "stdafx.h"
#include "CWantedLevelSync.h"

uint8_t CWantedLevelSync::m_nLocalOwnWantedLevel = 0;
uint8_t CWantedLevelSync::m_nLastSentOwnLevel = 0;
uint8_t CWantedLevelSync::m_nSharedMaxApplied = 0;

void CWantedLevelSync::Recalculate()
{
	uint8_t maxLevel = m_nLocalOwnWantedLevel;
	for (auto* player : CNetworkPlayerManager::m_pPlayers)
	{
		if (player && player->m_nWantedLevel > maxLevel)
			maxLevel = player->m_nWantedLevel;
	}

	CWanted* wanted = FindPlayerPed(0)->GetWanted();

	if (maxLevel > 0 && maxLevel > wanted->m_nWantedLevel)
	{
		wanted->SetWantedLevel(maxLevel);
		wanted->m_nLastTimeWantedDecreased = CTimer::m_snTimeInMilliseconds;
	}
	else if (maxLevel < m_nSharedMaxApplied)
	{
		wanted->SetWantedLevel(maxLevel);
	}

	m_nSharedMaxApplied = maxLevel;
}

void CWantedLevelSync::Trigger()
{
	if (!CNetwork::m_bConnected)
		return;

	CWanted* wanted = FindPlayerPed(0)->GetWanted();
	uint8_t currentGameLevel = (uint8_t)wanted->m_nWantedLevel;

	if (currentGameLevel != m_nSharedMaxApplied)
	{
		m_nLocalOwnWantedLevel = currentGameLevel;
	}

	if (m_nLocalOwnWantedLevel != m_nLastSentOwnLevel)
	{
		m_nLastSentOwnLevel = m_nLocalOwnWantedLevel;

		CPackets::PlayerWantedLevel packet{};
		packet.wantedLevel = m_nLocalOwnWantedLevel;
		CNetwork::SendPacket(CPacketsID::PLAYER_WANTED_LEVEL, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
	}

	Recalculate();
}

void CWantedLevelSync::ResetLocal()
{
	m_nLocalOwnWantedLevel = 0;
	m_nLastSentOwnLevel = 0;

	CPackets::PlayerWantedLevel packet{};
	packet.wantedLevel = 0;
	CNetwork::SendPacket(CPacketsID::PLAYER_WANTED_LEVEL, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);

	Recalculate();
}

void CWantedLevelSync::HandlePacket(void* data, int size)
{
	CPackets::PlayerWantedLevel* packet = (CPackets::PlayerWantedLevel*)data;

	if (auto networkPlayer = CNetworkPlayerManager::GetPlayer(packet->playerid))
	{
		networkPlayer->m_nWantedLevel = packet->wantedLevel;
	}

	Recalculate();
}

void CWantedLevelSync::OnPlayerDisconnected()
{
	Recalculate();
}

void CWantedLevelSync::OnPlayerRespawned(CNetworkPlayer* player)
{
	player->m_nWantedLevel = 0;
	Recalculate();
}
