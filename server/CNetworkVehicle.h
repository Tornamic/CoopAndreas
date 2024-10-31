#pragma once
#include "stdafx.h"

#include "CNetworkEntity.h"
#include "Modelable.h"
#include "VehicleSyncData.h"

class Driveable;

class CNetworkVehicle : public CNetworkEntity<VehicleSyncData>, public Modelable
{
public:
	std::array<Driveable*, 8> m_aOccupants;

	uint8_t m_damageManager_padding[23] = { 0 };

	std::vector<int32_t> m_aComponents;

	eNetworkEntityType GetType() const override { return eNetworkEntityType::NETWORK_ENTITY_VEHICLE; }
	
	CNetworkVehicle(uint16_t networkId, int16_t modelId) :
		CNetworkEntity(networkId), Modelable(modelId) {}
};

