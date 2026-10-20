#include "stdafx.h"
#include "CNetworkObject.h"
#include "CNetworkObjectManager.h"

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

CNetworkObject* CNetworkObjectManager::GetObject(CEntity* object)
{
    if (object == nullptr)
        return nullptr;

    for (int i = 0; i != m_pObjects.size(); i++)
    {
        if (m_pObjects[i]->m_pObject == object)
        {
            return m_pObjects[i];
        }
    }

    return nullptr;
}

void CNetworkObjectManager::Add(CNetworkObject* object)
{ CNetworkObjectManager::m_pObjects.push_back(object); }

void CNetworkObjectManager::Remove(CNetworkObject* object)
{
    auto it = std::find(m_pObjects.begin(), m_pObjects.end(), object);
    if (it != m_pObjects.end())
    {
        m_pObjects.erase(it);
    }
}

objectid_t CNetworkObjectManager::AllocateObjectId()
{
    assert(CNetworkPlayerManager::m_localPlayerId.valid());
    return objectid_t(CNetworkPlayerManager::m_localPlayerId, ms_nNextObjectId++);
}

void CNetworkObjectManager::ResetIdAllocator()
{ ms_nNextObjectId = 1; }

void CNetworkObjectManager::RemoveHostedUnused()
{
    for (auto it = CNetworkObjectManager::m_pObjects.begin(); it != CNetworkObjectManager::m_pObjects.end();)
    {
        if ((*it)->m_bSyncing && (*it)->m_pObject == nullptr)
        {
            delete *it;
            it = m_pObjects.erase(it);
            continue;
        }
        ++it;
    }
}

void CNetworkObjectManager::UpdateMovement()
{
    CNetworkObjectManager::RemoveHostedUnused();

    uint32_t currentTime = GetTickCount();

    for (auto pNetworkObject : m_pObjects)
    {
        CObject* pObject = pNetworkObject->m_pObject;
        if (pObject == nullptr || !pNetworkObject->m_bSyncing || pObject->m_matrix == nullptr)
            continue;

        if (!pNetworkObject->HasChangedSinceLastUpdate() &&
            currentTime - pNetworkObject->m_nLastUpdateTime < OBJECT_PERIODIC_UPDATE_INTERVAL)
            continue;

        Packets::Objects::ObjectUpdate packet{};
        packet.objectid = pNetworkObject->m_objectId;
        CNetworkObject::GetAttachState(pObject, packet.attach);
        packet.pos = pObject->m_matrix->pos;
        packet.right = pObject->m_matrix->right;
        packet.up = pObject->m_matrix->up;
        packet.moveSpeed = pObject->m_vecMoveSpeed;
        packet.turnSpeed = pObject->m_vecTurnSpeed;
        GetPacketFactory().Send(packet);

        pNetworkObject->StoreLastUpdate();
        pNetworkObject->m_nLastUpdateTime = currentTime;
    }
}
