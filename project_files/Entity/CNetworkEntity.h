#pragma once
#include "eNetworkEntityType.h"
#include "eEntityType.h"
#include <cstdint>

template <typename SyncDataType, typename EntityType>
class CNetworkEntity
{
private:
	uint16_t m_nNetworkId;
	SyncDataType m_syncData;

protected:
	CNetworkEntity(uint16_t networkId) : m_nNetworkId(networkId) {}

public:
	EntityType* m_pEntity = nullptr;

	virtual eNetworkEntityType GetType() const = 0;

	virtual void StreamIn() = 0;
	virtual void StreamOut() = 0;

	uint16_t GetId() const { return this->m_nNetworkId; }

	EntityType* GetEntity() { return this->m_pEntity; }

	SyncDataType& GetSyncData() { return this->m_syncData; }

	virtual void Create() = 0;
	virtual void Destroy() = 0;
};

