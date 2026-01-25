
#include "CVehicle.h"
#include "CVehicleManager.h"

std::vector<CVehicle*> CVehicleManager::m_pVehicles;

void CVehicleManager::Add(CVehicle* vehicle)
{
	m_pVehicles.push_back(vehicle);
}

void CVehicleManager::Remove(CVehicle* vehicle)
{
	auto it = std::find(m_pVehicles.begin(), m_pVehicles.end(), vehicle);
	if (it != m_pVehicles.end())
	{
		m_pVehicles.erase(it);
	}
}

CVehicle* CVehicleManager::GetVehicle(int vehicleid)
{
	for (int i = 0; i != m_pVehicles.size(); i++)
	{
		if (m_pVehicles[i]->m_nVehicleId == vehicleid)
		{
			return m_pVehicles[i];
		}
	}
	return nullptr;
}

int CVehicleManager::GetFreeID()
{
	for (int i = 0; i < 200; i++)
	{
		if (CVehicleManager::GetVehicle(i) == nullptr)
			return i;
	}

	return -1;
}

void CVehicleManager::RemoveAllHostedAndNotify(CPlayer* player)
{
	CVehiclePackets::VehicleRemove packet{};

	for (auto it = CVehicleManager::m_pVehicles.begin(); it != CVehicleManager::m_pVehicles.end();)
	{
		if ((*it)->m_pSyncer == player)
		{
			packet.vehicleid = (*it)->m_nVehicleId;
			CNetwork::SendPacketToAll(CPacketsID::VEHICLE_REMOVE, &packet, sizeof(packet), ENET_PACKET_FLAG_RELIABLE, player->m_pPeer);
			delete *it;
			it = CVehicleManager::m_pVehicles.erase(it);
		}
		else
		{
			++it;
		}
	}
}