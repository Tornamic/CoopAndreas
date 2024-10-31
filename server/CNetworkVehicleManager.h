#pragma once
#include "stdafx.h"

#include "CNetworkEntityManager.h"

class CNetworkVehicle;

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

	CNetworkVehicleManager(const CNetworkVehicleManager&) = delete;
	CNetworkVehicleManager& operator=(const CNetworkVehicleManager&) = delete;
};

