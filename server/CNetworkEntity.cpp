#include "CNetworkEntity.h"
#include "CPackets.h"

bool CNetworkEntity::IsStreamedFor(CNetworkPlayer* forPlayer)
{
    auto& entities = forEntity->GetStreamedFor(); 
    return std::find(entities.begin(), entities.end(), this) != entities.end();
}

void CNetworkEntity::AddStreamedFor(CNetworkPlayer* forPlayer)
{
    this->GetStreamedFor().push_back(forEntity);

    CPackets::EntityStream packet{};
    packet.m_nNetworkId = this->GetId();
    packet.m_nEntityType = this->GetType();

}

void CNetworkEntity::RemoveStreamedFor(CNetworkPlayer* forPlayer)
{
    auto& entities = this->GetStreamedFor();
    entities.erase(std::remove(entities.begin(), entities.end(), forEntity), entities.end());
}
