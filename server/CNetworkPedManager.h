#pragma once
#include "stdafx.h"

#include "CNetworkEntityManager.h"

class CNetworkPed;

class CNetworkPedManager : public CNetworkEntityManager<CNetworkPed>
{
private:
	CNetworkPedManager() {}
public:
	static CNetworkPedManager& Instance()
	{
		static CNetworkPedManager instance;
		return instance;
	}

	uint16_t GetMaxCount() const override { return 200; }

	CNetworkPedManager(const CNetworkPedManager&) = delete;
	CNetworkPedManager& operator=(const CNetworkPedManager&) = delete;
};

