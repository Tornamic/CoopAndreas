#include "stdafx.h"

std::vector<CNetworkPed*> CNetworkPedManager::m_pPeds;

CNetworkPed* CNetworkPedManager::GetPed(int pedid)
{
	for (int i = 0; i != m_pPeds.size(); i++)
	{
		if (m_pPeds[i]->m_nPedId == pedid)
		{
			return m_pPeds[i];
		}
	}
	return nullptr;
}

CNetworkPed* CNetworkPedManager::GetPed(CPed* ped)
{
	for (int i = 0; i != m_pPeds.size(); i++)
	{
		if (m_pPeds[i]->m_pPed == ped)
		{
			return m_pPeds[i];
		}
	}

	return nullptr;
}

CNetworkPed* CNetworkPedManager::GetPed(CEntity* entity)
{
	for (int i = 0; i != m_pPeds.size(); i++)
	{
		if (m_pPeds[i]->m_pPed == entity)
		{
			return m_pPeds[i];
		}
	}

	return nullptr;
}

int CNetworkPedManager::GetFreeID()
{
	if (!CLocalPlayer::m_bIsHost)
		return -1;

	for (int i = 0; i != MAX_SERVER_PEDS; i++)
	{
		if (CNetworkPedManager::GetPed(i) == nullptr)
			return i;
	}

	return -1;
}

void CNetworkPedManager::Add(CNetworkPed* ped)
{
	CNetworkPedManager::m_pPeds.push_back(ped);
}

void CNetworkPedManager::Remove(CNetworkPed* ped)
{
	auto it = std::find(m_pPeds.begin(), m_pPeds.end(), ped);
	if (it != m_pPeds.end())
	{
		m_pPeds.erase(it);
	}
}

void CNetworkPedManager::Update()
{
	if (CLocalPlayer::m_bIsHost)
	{
		for (int i = 0; i != m_pPeds.size(); i++)
		{
			if (m_pPeds[i]->m_pPed == nullptr)
				continue;

			CPackets::PedOnFoot* packet = CPacketHandler::PedOnFoot__Collect(m_pPeds[i]);
			CNetwork::SendPacket(CPacketsID::PED_ONFOOT, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED);
		}
	}
}
