#pragma once
#include "stdafx.h"

#include "eNetworkEntityType.h"
#include "CNetworkPlayer.h"

class CNetworkEntity 
{
protected:
	CNetworkEntity(uint16_t networkId) : m_nNetworkId(networkId) {}
private:
	uint16_t m_nNetworkId = 0;
	std::vector<CNetworkEntity*> m_streamedFor;
public:
	virtual eNetworkEntityType GetType() const = 0;

	uint16_t GetId() const { return m_nNetworkId; }

	std::vector<CNetworkEntity*>& GetStreamedFor() { return this->m_streamedFor; }
	bool IsStreamedFor(CNetworkPlayer* forPlayer);
	void AddStreamedFor(CNetworkPlayer* forPlayer);
	void RemoveStreamedFor(CNetworkPlayer* forPlayer);
};

