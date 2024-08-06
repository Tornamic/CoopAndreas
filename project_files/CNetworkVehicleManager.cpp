#include "stdafx.h"

std::vector<CNetworkVehicle*> CNetworkVehicleManager::m_pVehicles;

CNetworkVehicle* CNetworkVehicleManager::GetVehicle(int vehicleid)
{
	return nullptr;
}

CNetworkVehicle* CNetworkVehicleManager::GetVehicle(CVehicle* vehicle)
{
	return nullptr;
}

int CNetworkVehicleManager::GetFreeID()
{
	if (!CLocalPlayer::m_bIsHost)
		return -1;

	for (int i = 0; i != MAX_SERVER_VEHICLES; i++)
	{
		if (CNetworkVehicleManager::GetVehicle(i) == nullptr)
			return i;
	}

	return -1;
}

void CNetworkVehicleManager::Add(CNetworkVehicle* vehicle)
{
	CNetworkVehicleManager::m_pVehicles.push_back(vehicle);
}

void CNetworkVehicleManager::Remove(CNetworkVehicle* vehicle)
{
	auto it = std::find(m_pVehicles.begin(), m_pVehicles.end(), vehicle);
	if (it != m_pVehicles.end())
	{
		m_pVehicles.erase(it);
	}
}