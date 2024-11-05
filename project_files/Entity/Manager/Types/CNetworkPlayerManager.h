#pragma once

#include "../../Types/CNetworkPlayer.h"
#include "../CNetworkEntityManager.h"

class CEntity;

class CNetworkPlayerManager final : public CNetworkEntityManager<CNetworkPlayer>
{
private:
	CNetworkPlayerManager() {}

public:
	static CNetworkPlayerManager& Instance()
	{
		static CNetworkPlayerManager instance;
		return instance;
	}

	uint16_t GetMaxCount() const override { return 100; }

	CNetworkPlayerManager(const CNetworkPlayerManager&) = delete;
	CNetworkPlayerManager& operator=(const CNetworkPlayerManager&) = delete;
};

