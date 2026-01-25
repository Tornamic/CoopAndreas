#include "stdafx.h"
#include "CNetworkVehicle.h"

std::vector<CNetworkVehicle*> CNetworkVehicleManager::m_pVehicles;
CNetworkVehicle* CNetworkVehicleManager::m_apTempVehicles[255];

CNetworkVehicle* CNetworkVehicleManager::GetVehicle(int vehicleid)
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

CNetworkVehicle* CNetworkVehicleManager::GetVehicle(CVehicle* vehicle)
{
	for (int i = 0; i != m_pVehicles.size(); i++)
	{
		if (m_pVehicles[i]->m_pVehicle == vehicle)
		{
			return m_pVehicles[i];
		}
	}
	
	return nullptr;
}

CNetworkVehicle* CNetworkVehicleManager::GetVehicle(CEntity* vehicle)
{
	for (int i = 0; i != m_pVehicles.size(); i++)
	{
		if (m_pVehicles[i]->m_pVehicle == vehicle)
		{
			return m_pVehicles[i];
		}
	}

	return nullptr;
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

void CNetworkVehicleManager::UpdateDriver(CVehicle* vehicle)
{
	if (auto networkVehicle = CNetworkVehicleManager::GetVehicle(vehicle))
	{
		CPackets::VehicleDriverUpdate* packet = CPacketHandler::VehicleDriverUpdate__Collect(networkVehicle);
		CNetwork::SendPacket(CPacketsID::VEHICLE_DRIVER_UPDATE, packet, sizeof *packet);
		delete packet;
	}
}

void CNetworkVehicleManager::UpdateIdle()
{
	CNetworkVehicleManager::RemoveHostedUnused();

	CMassPacketBuilder builder;

	for (int i = 0; i != m_pVehicles.size(); i++)
	{
		if (m_pVehicles[i]->m_pVehicle == nullptr)
			continue;

		if (m_pVehicles[i]->m_bSyncing && !m_pVehicles[i]->HasDriver())
		{
			CPackets::VehicleIdleUpdate* packet = CPacketHandler::VehicleIdleUpdate__Collect(m_pVehicles[i]);
			builder.AddPacket(CPacketsID::VEHICLE_IDLE_UPDATE, packet, sizeof *packet);
			delete packet;
		}
	}

	builder.Send();
}

void CNetworkVehicleManager::UpdatePassenger(CVehicle* vehicle, CPlayerPed* localPlayer)
{
	CNetworkVehicle* networkVehicle = CNetworkVehicleManager::GetVehicle(vehicle);
	CPackets::VehiclePassengerUpdate* packet = CPacketHandler::VehiclePassengerUpdate__Collect(networkVehicle, localPlayer);
	CNetwork::SendPacket(CPacketsID::VEHICLE_PASSENGER_UPDATE, packet, sizeof * packet);
	delete packet;
}

unsigned char CNetworkVehicleManager::AddToTempList(CNetworkVehicle* networkVehicle)
{
	for (unsigned char i = 0; i < 255; i++)
	{
		if (m_apTempVehicles[i] == nullptr)
		{
			m_apTempVehicles[i] = networkVehicle;
			return i;
		}
	}

	return 255;
}

void CNetworkVehicleManager::RemoveHostedUnused()
{
	for (auto it = CNetworkVehicleManager::m_pVehicles.begin(); it != CNetworkVehicleManager::m_pVehicles.end();)
	{
		if ((*it)->m_bSyncing)
		{
			CVehicle* vehicle = (*it)->m_pVehicle;
			if (!IsVehiclePointerValid(vehicle))
			{
				delete* it;
				it = m_pVehicles.erase(it);
				continue;
			}
		}
		++it;
	}
}