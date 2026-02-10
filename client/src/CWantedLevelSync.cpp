#include "stdafx.h"
#include "CWantedLevelSync.h"

uint8_t CWantedLevelSync::m_nLocalOwnWantedLevel = 0;
uint8_t CWantedLevelSync::m_nLastSentOwnLevel = 0;
uint8_t CWantedLevelSync::m_nLastAppliedLevel = 0;

void CWantedLevelSync::Recalculate()
{
	CPlayerPed* localPed = FindPlayerPed(0);
	if (!localPed)
		return;

	uint8_t maxLevel = m_nLocalOwnWantedLevel;

	CVehicle* localVehicle = (localPed->m_nPedFlags.bInVehicle && localPed->m_pVehicle)
		? localPed->m_pVehicle : nullptr;

	if (localVehicle)
	{
		for (auto* player : CNetworkPlayerManager::m_pPlayers)
		{
			if (!player || !player->m_pPed || player->m_nWantedLevel <= maxLevel)
				continue;

			if (player->m_pPed->m_nPedFlags.bInVehicle
				&& player->m_pPed->m_pVehicle == localVehicle)
				maxLevel = player->m_nWantedLevel;
		}
	}

	CWanted* wanted = localPed->GetWanted();
	if (!wanted)
		return;

	if (maxLevel > 0 && maxLevel > wanted->m_nWantedLevel)
	{
		wanted->SetWantedLevel(maxLevel);
		wanted->m_nLastTimeWantedDecreased = CTimer::m_snTimeInMilliseconds;
	}
	else if (maxLevel > m_nLocalOwnWantedLevel && maxLevel == wanted->m_nWantedLevel)
	{
		wanted->m_nLastTimeWantedDecreased = CTimer::m_snTimeInMilliseconds;
	}

	m_nLastAppliedLevel = maxLevel;
}

void CWantedLevelSync::Trigger()
{
	if (!CNetwork::m_bConnected)
		return;

	CPlayerPed* localPed = FindPlayerPed(0);
	if (!localPed)
		return;

	CWanted* wanted = localPed->GetWanted();
	if (!wanted)
		return;

	uint8_t currentGameLevel = (uint8_t)wanted->m_nWantedLevel;

	if (currentGameLevel != m_nLastAppliedLevel)
	{
		m_nLocalOwnWantedLevel = currentGameLevel;

		if (currentGameLevel < m_nLastAppliedLevel
			&& localPed->m_nPedFlags.bInVehicle && localPed->m_pVehicle)
		{
			for (auto* player : CNetworkPlayerManager::m_pPlayers)
			{
				if (player && player->m_pPed
					&& player->m_pPed->m_nPedFlags.bInVehicle
					&& player->m_pPed->m_pVehicle == localPed->m_pVehicle
					&& player->m_nWantedLevel > currentGameLevel)
				{
					player->m_nWantedLevel = currentGameLevel;
				}
			}
		}
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
