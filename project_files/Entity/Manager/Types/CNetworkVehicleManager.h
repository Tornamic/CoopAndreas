#pragma once

#include <CVehicle.h>

#include "../../Types/CNetworkVehicle.h"
#include "../CNetworkEntityManager.h"
#include <CPlayerPed.h>

class CNetworkVehicleManager final : public CNetworkEntityManager<CNetworkVehicle>
{
private:
	CNetworkVehicleManager() {}

public:
	static CNetworkVehicleManager& Instance()
	{
		static CNetworkVehicleManager instance;
		return instance;
	}

	uint16_t GetMaxCount() const override { return 200; }

    void UpdateDriver(CVehicle* vehicle);
    void UpdateIdle();
    void UpdatePassenger(CVehicle* vehicle, CPlayerPed* localPlayer);

	CNetworkVehicleManager(const CNetworkVehicleManager&) = delete;
	CNetworkVehicleManager& operator=(const CNetworkVehicleManager&) = delete;
};

