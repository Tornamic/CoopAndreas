#pragma once
#include "stdafx.h"

#include "eNetworkEntityType.h"
class CNetworkPlayer;

template <typename SyncDataType>
class CNetworkEntity
{
private:
    uint16_t m_nNetworkId = 0;
    std::vector<CNetworkPlayer*> m_streamedFor;
    SyncDataType m_syncData;

protected:
    CNetworkEntity(uint16_t networkId) : m_nNetworkId(networkId) {}

public:
    virtual eNetworkEntityType GetType() const = 0;

    uint16_t GetId() const { return this->m_nNetworkId; }

    SyncDataType& GetSyncData() { return this->m_syncData; }

    std::vector<CNetworkPlayer*>& GetStreamedFor() { return this->m_streamedFor; }

    bool IsStreamedFor(CNetworkPlayer* forPlayer)
    {
        auto& entities = this->GetStreamedFor();
        return std::find(entities.begin(), entities.end(), forPlayer) != entities.end();
    }

    void AddStreamedFor(CNetworkPlayer* forPlayer)
    {
        this->GetStreamedFor().push_back(forPlayer);
    }

    void RemoveStreamedFor(CNetworkPlayer* forPlayer)
    {
        auto& entities = this->GetStreamedFor();
        entities.erase(std::remove(entities.begin(), entities.end(), forPlayer), entities.end());
    }
};
