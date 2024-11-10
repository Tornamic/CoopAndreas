#pragma once
#include "stdafx.h"

#include "eNetworkEntityType.h"
#include "CNetworkEntity.h"
#include "Driveable.h"
#include "PlayerSyncData.h"

class CNetworkPlayer final : public CNetworkEntity<PlayerSyncData>, public Driveable
{
private:
	char m_name[32 + 1] = { 0 };

public:
	ENetPeer* m_pPeer = nullptr;
	bool m_bIsHost = false;
	
	ENetPeer* GetPeer() { return m_pPeer; }

	const char* GetName() const { return m_name; }
	void SetName(const char* name) { strncpy_s(m_name, name, sizeof(m_name) - 1); m_name[sizeof(m_name) - 1] = '\0'; }

	bool IsHost() { return m_bIsHost; }

	eNetworkEntityType GetType() const override { return eNetworkEntityType::NETWORK_ENTITY_PLAYER; }

	CNetworkPlayer(uint16_t networkId, ENetPeer* peer) : 
		CNetworkEntity(networkId), Driveable(), m_pPeer(peer) 
	{
		std::cout << "CNetworkPlayer constructed: " << m_pPeer << std::endl;
	}
};
