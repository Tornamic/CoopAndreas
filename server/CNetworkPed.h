#pragma once
#include "stdafx.h"

#include "CNetworkEntity.h"
#include "Modelable.h"
#include "Driveable.h"

class CNetworkPed final : public CNetworkEntity, Modelable, Driveable
{
private:
	uint8_t m_nPedType; 
	unsigned char m_nCreatedBy;

public:
	eNetworkEntityType GetType() const override { return eNetworkEntityType::NETWORK_ENTITY_PED; }

	CNetworkPed(uint16_t networkId, int16_t modelId, uint8_t pedType, unsigned char createdBy) :
		CNetworkEntity(networkId), Modelable(modelId), Driveable(), m_nPedType(pedType), m_nCreatedBy(createdBy) {}
};