
#include "CPedManager.h"

std::vector<CPed*> CPedManager::m_pPeds;

void CPedManager::Add(CPed* ped)
{
	m_pPeds.push_back(ped);
}

void CPedManager::Remove(CPed* ped)
{
	auto it = std::find(m_pPeds.begin(), m_pPeds.end(), ped);
	//std::find()
	if (it != m_pPeds.end())
	{
		m_pPeds.erase(it);
	}
}

CPed* CPedManager::GetPed(int pedid)
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

int CPedManager::GetFreeId()
{
	for (int i = 0; i < 300; i++)
	{
		if (CPedManager::GetPed(i) == nullptr)
			return i;
	}

	return -1;
}

void CPedManager::RemoveAllHostedAndNotify(CPlayer* player)
{
	CPedPackets::PedRemove packet{};

	for (auto it = CPedManager::m_pPeds.begin(); it != CPedManager::m_pPeds.end();)
	{
		if ((*it)->m_pSyncer == player)
		{
			packet.pedid = (*it)->m_nPedId;
			CNetwork::SendPacketToAll(CPacketsID::PED_REMOVE, &packet, sizeof(packet), ENET_PACKET_FLAG_RELIABLE, player->m_pPeer);
			delete* it;
			it = CPedManager::m_pPeds.erase(it);
		}
		else
		{
			++it;
		}
	}
}