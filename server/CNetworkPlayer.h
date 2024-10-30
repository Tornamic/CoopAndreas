#pragma once
#include "stdafx.h"

#include "CNetworkEntity.h"
#include "Driveable.h"

class CNetworkPlayer final : public CNetworkEntity, Driveable
{
private:
	char m_name[32 + 1] = { 0 };

public:
	ENetPeer* m_pPeer = nullptr;
	bool m_bIsHost = false;
	
	ENetPeer* GetPeer() { return m_pPeer; }

	char* GetName() { return m_name; }
	void SetName(char* name) { strcpy(m_name, name); }

	bool IsHost() { return m_bIsHost; }
	void SetHost(bool host) { m_bIsHost = host; }

	eNetworkEntityType GetType() const override { return eNetworkEntityType::NETWORK_ENTITY_PLAYER; }

	CNetworkPlayer(uint16_t networkId, ENetPeer* peer) : 
		CNetworkEntity(networkId), Driveable(), m_pPeer(peer) {}
};
