#pragma once

#include "../../Types/CNetworkPed.h"
#include "../CNetworkEntityManager.h"

class CNetworkPedManager final : public CNetworkEntityManager<CNetworkPed>
{
private:
	CNetworkPedManager() {}

public:
	static CNetworkPedManager& Instance()
	{
		static CNetworkPedManager instance;
		return instance;
	}

public:
	uint16_t GetMaxCount() const override { return 100; }

	void Update();
	void Process();
	void AssignHost();

	CNetworkPedManager(const CNetworkPedManager&) = delete;
	CNetworkPedManager& operator=(const CNetworkPedManager&) = delete;
};

