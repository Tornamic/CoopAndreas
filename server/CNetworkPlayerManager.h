#pragma once
#include "stdafx.h"

#include "CNetworkEntityManager.h"
#include "CNetworkPlayer.h"

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

	CNetworkPlayer* Get(ENetPeer* peer);
	CNetworkPlayer* GetHost();
	void AssignHostToFirstPlayer();

	uint16_t GetMaxCount() const override { return 100; }

	CNetworkPlayerManager(const CNetworkPlayerManager&) = delete;
	CNetworkPlayerManager& operator=(const CNetworkPlayerManager&) = delete;
};

