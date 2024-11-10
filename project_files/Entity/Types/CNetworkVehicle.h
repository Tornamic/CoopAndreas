#pragma once
#include "../Abstract/Hostable.h"
#include "../Abstract/Modelable.h"
#include "../CNetworkEntity.h"
#include "../Data/VehicleSyncData.h"
#include <CVehicle.h>

class CNetworkVehicle : public CNetworkEntity<VehicleSyncData, CVehicle>, public Modelable, public Hostable
{
public:
	static CNetworkVehicle* NotifyNew(CVehicle* vehicle);

	CNetworkVehicle(uint16_t networkId, int16_t modelId) :
		CNetworkEntity(networkId), Modelable(modelId) {}

	char m_nPaintJob = -1;
	float m_fAimHorizontal = 0.0f;
	float m_fAimVertical = 0.0f;

	~CNetworkVehicle();
	bool HasDriver();

	eNetworkEntityType GetType() const override { return eNetworkEntityType::NETWORK_ENTITY_VEHICLE; }
	void StreamIn() override;
	void StreamOut() override;
	void Create() override;
	void Destroy() override;
};

