#include "stdafx.h"
#include "network/CNetworkEntitySerializer.h"

CEntity* CNetworkEntitySerializer::GetEntity()
{
	if (entityType == NETWORK_ENTITY_TYPE_PED)
	{
		if (auto networkPed = CNetworkPedManager::GetPed(pedid))
		{
			if (auto ped = networkPed->m_pPed)
			{
				return ped;
			}
		}
	}
	else if (entityType == NETWORK_ENTITY_TYPE_VEHICLE)
	{
		if (auto networkVehicle = CNetworkVehicleManager::GetVehicle(vehicleid))
		{
			if (auto vehicle = networkVehicle->m_pVehicle)
			{
				return vehicle;
			}
		}
	}
	else if (entityType == NETWORK_ENTITY_TYPE_OBJECT)
	{
		if (auto networkObject = CNetworkObjectManager::GetObject(objectid))
		{
			if (auto object = networkObject->m_pObject)
			{
				return object;
			}
		}
	}
	else if (entityType == NETWORK_ENTITY_TYPE_PLAYER)
	{
		if (CNetworkPlayerManager::m_localPlayerId == playerid)
		{
			return FindPlayerPed(0);
		}
		if (auto networkPlayer = CNetworkPlayerManager::GetPlayer(playerid))
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
	playerid = playerid_t();
	vehicleid = vehicleid_t();
	pedid = pedid_t();
	objectid = objectid_t();

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
				playerid = CNetworkPlayerManager::m_localPlayerId;
			}

			if (auto networkPlayer = CNetworkPlayerManager::GetPlayer(entity))
			{
				playerid = networkPlayer->m_playerId;
			}
		}
		else
		{
			entityType = NETWORK_ENTITY_TYPE_PED;
			if (auto networkPed = CNetworkPedManager::GetPed(entity))
			{
				pedid = networkPed->m_pedId;
			}
		}
	}
	else if (entity->m_nType == ENTITY_TYPE_VEHICLE)
	{
		entityType = NETWORK_ENTITY_TYPE_VEHICLE;
		if (auto networkVehicle = CNetworkVehicleManager::GetVehicle(entity))
		{
			vehicleid = networkVehicle->m_vehicleId;
		}
	}
	else if (entity->m_nType == ENTITY_TYPE_OBJECT)
	{
		entityType = NETWORK_ENTITY_TYPE_OBJECT;
		if (auto networkObject = CNetworkObjectManager::GetObject(entity))
		{
			objectid = networkObject->m_objectId;
		}
	}
}