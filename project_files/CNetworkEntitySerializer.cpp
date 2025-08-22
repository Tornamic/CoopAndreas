#include "stdafx.h"
#include "CNetworkEntitySerializer.h"

CEntity* CNetworkEntitySerializer::GetEntity()
{
	if (entityType == NETWORK_ENTITY_TYPE_PED)
	{
		if (auto networkPed = CNetworkPedManager::GetPed(entityId))
		{
			if (auto ped = networkPed->m_pPed)
			{
				return ped;
			}
		}
	}
	else if (entityType == NETWORK_ENTITY_TYPE_VEHICLE)
	{
		if (auto networkVehicle = CNetworkVehicleManager::GetVehicle(entityId))
		{
			if (auto vehicle = networkVehicle->m_pVehicle)
			{
				return vehicle;
			}
		}
	}
	else if (entityType == NETWORK_ENTITY_TYPE_PLAYER)
	{
		if (auto networkPlayer = CNetworkPlayerManager::GetPlayer(entityId))
		{
			if (auto ped = networkPlayer->m_pPed)
			{
				return ped;
			}
		}
	}

	return nullptr;
}

void CNetworkEntitySerializer::SetEntity(CEntity* entity)
{
	entityType = NETWORK_ENTITY_TYPE_NOTHING;

	if (entity == nullptr)
	{
		return;
	}

	if (entity->m_nType == ENTITY_TYPE_PED)
	{
		if (((CPed*)entity)->IsPlayer())
		{
			entityType = NETWORK_ENTITY_TYPE_PLAYER;
			if (entity == FindPlayerPed(0))
			{
				entityId = CNetworkPlayerManager::m_nMyId;
			}

			if (auto networkPlayer = CNetworkPlayerManager::GetPlayer(entity))
			{
				entityId = networkPlayer->m_iPlayerId;
			}
		}
		else
		{
			entityType = NETWORK_ENTITY_TYPE_PED;
			if (auto networkPed = CNetworkPedManager::GetPed(entity))
			{
				entityId = networkPed->m_nPedId;
			}
		}
	}
	else if (entity->m_nType == ENTITY_TYPE_VEHICLE)
	{
		entityType = NETWORK_ENTITY_TYPE_VEHICLE;
		if (auto networkVehicle = CNetworkVehicleManager::GetVehicle(entity))
		{
			entityId = networkVehicle->m_nVehicleId;
		}
	}
}