#pragma once
#include "stdafx.h"

#include "eNetworkEntityType.h"
#include "CNetworkEntity.h"
#include "Driveable.h"
#include "Modelable.h"
#include "PedSyncData.h"

class CNetworkPed final : public CNetworkEntity<PedSyncData>, public Modelable, public Driveable
{
private:
	uint8_t m_nPedType; 
	uint8_t m_nCreatedBy;

public:
	uint8_t GetPedType() { return m_nPedType; }
	uint8_t GetCreatedBy() { return m_nCreatedBy; }

	eNetworkEntityType GetType() const override { return eNetworkEntityType::NETWORK_ENTITY_PED; }

	CNetworkPed(uint16_t networkId, int16_t modelId, uint8_t pedType, uint8_t createdBy) :
		CNetworkEntity(networkId), Modelable(modelId), Driveable(), m_nPedType(pedType), m_nCreatedBy(createdBy) {}
};