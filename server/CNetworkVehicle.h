#pragma once
#include "stdafx.h"

#include "CNetworkEntity.h"
#include "Modelable.h"
#include "Driveable.h"

class CNetworkVehicle : public CNetworkEntity, Modelable
{
public:
	std::array<Driveable*, 8> m_aOccupants = { nullptr };

	uint8_t m_nPrimaryColor;
	uint8_t m_nSecondaryColor;
	
	uint8_t m_damageManager_padding[23] = { 0 };

	std::vector<int> m_pComponents;

	CNetworkVehicle(uint16_t networkId, int16_t modelId) :
		CNetworkEntity(networkId), Modelable(modelId) {}
};

