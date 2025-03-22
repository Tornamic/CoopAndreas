#include "stdafx.h"
#include "CNetworkEntityBlip.h"

void CNetworkEntityBlip::UpdateEntityBlip(CPackets::UpdateEntityBlip* packet)
{
	int handle = -1;
	switch (packet->entityType)
	{
	case eNetworkEntityType::NETWORK_ENTITY_TYPE_PED:
		if (auto networkPed = CNetworkPedManager::GetPed(packet->entityId))
		{
			if (auto ped = networkPed->m_pPed)
			{
				if (networkPed->m_nBlipHandle == -1)
				{
					networkPed->m_nBlipHandle = CRadar::SetEntityBlip(BLIP_CHAR, CPools::GetPedRef(ped), 0, (eBlipDisplay)packet->display);
				}
				handle = networkPed->m_nBlipHandle;
			}
		}
		break;
	case eNetworkEntityType::NETWORK_ENTITY_TYPE_VEHICLE:
		if (auto networkVehicle = CNetworkVehicleManager::GetVehicle(packet->entityId))
		{
			if (auto vehicle = networkVehicle->m_pVehicle)
			{
				if (networkVehicle->m_nBlipHandle == -1)
				{
					networkVehicle->m_nBlipHandle = CRadar::SetEntityBlip(BLIP_CAR, CPools::GetVehicleRef(vehicle), 0, (eBlipDisplay)packet->display);
				}
				handle = networkVehicle->m_nBlipHandle;
			}
		}
		break;
	}

	if (handle >= 0)
	{
		//CChat::AddMessage("%d %d %d %d", packet->scale, packet->color, packet->display, packet->isFriendly);
		CRadar::ChangeBlipScale(handle, packet->scale);
		//CRadar::ChangeBlipColour(handle, packet->color);
		CRadar::ChangeBlipDisplay(handle, (eBlipDisplay)packet->display);
		CRadar::SetBlipFriendly(handle, packet->isFriendly);
	}
}

void CNetworkEntityBlip::RemoveEntityBlip(CPackets::RemoveEntityBlip* packet)
{
	switch (packet->entityType)
	{
	case eNetworkEntityType::NETWORK_ENTITY_TYPE_PED:
		if (auto networkPed = CNetworkPedManager::GetPed(packet->entityId))
		{
			if (auto ped = networkPed->m_pPed)
			{
				if (networkPed->m_nBlipHandle >= 0)
				{
					CRadar::ClearBlip(networkPed->m_nBlipHandle);
				}
				networkPed->m_nBlipHandle = -1;
			}
		}
		break;
	case eNetworkEntityType::NETWORK_ENTITY_TYPE_VEHICLE:
		if (auto networkVehicle = CNetworkVehicleManager::GetVehicle(packet->entityId))
		{
			if (auto vehicle = networkVehicle->m_pVehicle)
			{
				if (networkVehicle->m_nBlipHandle >= 0)
				{
					CRadar::ClearBlip(networkVehicle->m_nBlipHandle);
				}
				networkVehicle->m_nBlipHandle = -1;
			}
		}
		break;
	}
}

void CNetworkEntityBlip::ClearEntityBlips()
{
	for (auto networkPed : CNetworkPedManager::m_pPeds)
	{
		if (networkPed->m_nBlipHandle == -1
			|| !networkPed->m_pPed)
		{
			continue;
		}

		CRadar::ClearBlipForEntity(eBlipType::BLIP_CHAR, CPools::GetPedRef(networkPed->m_pPed));
		networkPed->m_nBlipHandle = -1;
	}

	for (auto networkVehicle : CNetworkVehicleManager::m_pVehicles)
	{
		if (networkVehicle->m_nBlipHandle == -1
			|| !networkVehicle->m_pVehicle)
		{
			continue;
		}
		
		CRadar::ClearBlipForEntity(eBlipType::BLIP_CAR, CPools::GetVehicleRef(networkVehicle->m_pVehicle));
		networkVehicle->m_nBlipHandle = -1;
	}
}
