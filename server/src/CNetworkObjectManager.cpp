#include "stdafx.h"

std::vector<CNetworkObject*> CNetworkObjectManager::m_pObjects;

void CNetworkObjectManager::Add(CNetworkObject* object)
{ m_pObjects.push_back(object); }

void CNetworkObjectManager::Remove(CNetworkObject* object)
{
    auto it = std::find(m_pObjects.begin(), m_pObjects.end(), object);
    if (it != m_pObjects.end())
    {
        m_pObjects.erase(it);
    }
}

CNetworkObject* CNetworkObjectManager::GetObject(objectid_t objectid)
{
    if (!objectid.valid())
        return nullptr;

    for (int i = 0; i != m_pObjects.size(); i++)
    {
        if (m_pObjects[i]->m_objectId == objectid)
        {
            return m_pObjects[i];
        }
    }
    return nullptr;
}

void CNetworkObjectManager::RemoveAllHostedAndNotify(CNetworkPlayer* player)
{
    for (auto it = CNetworkObjectManager::m_pObjects.begin(); it != CNetworkObjectManager::m_pObjects.end();)
    {
        if ((*it)->m_pSyncer == player)
        {
            Packets::Objects::ObjectRemove packet{};
            packet.objectid = (*it)->m_objectId;
            GetPacketFactory().SendToAll(packet, player);

            delete *it;
            it = CNetworkObjectManager::m_pObjects.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
