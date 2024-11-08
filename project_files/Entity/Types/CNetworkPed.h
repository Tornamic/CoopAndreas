#pragma once

#include "../../stdafx.h"
#include "../Abstract/Hostable.h"
#include "../Abstract/Modelable.h"
#include "../CNetworkEntity.h"
#include "../Data/PedSyncData.h"

class CNetworkPed final : public CNetworkEntity<PedSyncData, CPed>, public Modelable, public Hostable
{
public:
	CNetworkPed(uint16_t networkId, int16_t modelId) :
		CNetworkEntity(networkId), Modelable(modelId) {}

	ePedType m_nPedType;
	uint8_t m_nCreatedBy;
	CAutoPilot m_autoPilot;
	float m_fGasPedal = 0.0f;
	float m_fBreakPedal = 0.0f;
	float m_fSteerAngle = 0.0f;

	~CNetworkPed();

	eNetworkEntityType GetType() const override { return eNetworkEntityType::NETWORK_ENTITY_PED; }
	void StreamIn() override;
	void StreamOut() override;
	void Create() override;
	void Destroy() override;
};

