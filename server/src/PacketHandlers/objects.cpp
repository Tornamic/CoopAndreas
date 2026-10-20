#include "network/packet_types.h"
#include "stdafx.h"
#include "network/packet_handler.h"
#include "network/packets/objects.h"

PACKET_HANDLER(ePacketType::OBJECT_SPAWN, Packets::Objects::ObjectSpawn* pObjectSpawn, CNetworkPlayer* pNetworkPlayer)
{
    if (pObjectSpawn->objectid.creator() != pNetworkPlayer->m_playerId)
    {
        logger::warn("%s tried to create an object with someone else's id %s", pNetworkPlayer->GetName().c_str(),
            to_string(pObjectSpawn->objectid).c_str());
        return;
    }

    if (CNetworkObjectManager::GetObject(pObjectSpawn->objectid) != nullptr)
    {
        logger::warn("%s tried to create an object with an id that is already in use %s",
            pNetworkPlayer->GetName().c_str(), to_string(pObjectSpawn->objectid).c_str());
        return;
    }

    GetPacketFactory().SendToAll(*pObjectSpawn, pNetworkPlayer);

    CNetworkObject* object = new CNetworkObject(*pObjectSpawn);
    object->m_pSyncer = pNetworkPlayer;

    CNetworkObjectManager::Add(object);
}

PACKET_HANDLER(
    ePacketType::OBJECT_REMOVE, Packets::Objects::ObjectRemove* pObjectRemove, CNetworkPlayer* pNetworkPlayer)
{
    if (auto object = CNetworkObjectManager::GetObject(pObjectRemove->objectid))
    {
        if (object->m_pSyncer == pNetworkPlayer)
        {
            GetPacketFactory().SendToAll(*pObjectRemove, pNetworkPlayer);

            CNetworkObjectManager::Remove(object);
            delete object;
        }
        else
        {
            logger::warn("%s tried to delete someone else's object", pNetworkPlayer->GetName().c_str());
        }
    }
}

PACKET_HANDLER(
    ePacketType::OBJECT_UPDATE, Packets::Objects::ObjectUpdate* pObjectUpdate, CNetworkPlayer* pNetworkPlayer)
{
    if (auto object = CNetworkObjectManager::GetObject(pObjectUpdate->objectid))
    {
        if (object->m_pSyncer == pNetworkPlayer)
        {
            object->m_vecPosition = pObjectUpdate->pos;
            object->m_vecRight = pObjectUpdate->right;
            object->m_vecUp = pObjectUpdate->up;
            object->m_lastUpdate = *pObjectUpdate;
            object->m_bHasUpdate = true;
            GetPacketFactory().SendToAll(*pObjectUpdate, pNetworkPlayer);
        }
        else
        {
            logger::warn("%s tried to update someone else's object", pNetworkPlayer->GetName().c_str());
        }
    }
}

PACKET_HANDLER(ePacketType::OBJECT_DAMAGE_REQUEST, Packets::Objects::ObjectDamageRequest* pObjectDamageRequest,
    CNetworkPlayer* pNetworkPlayer)
{
    if (auto object = CNetworkObjectManager::GetObject(pObjectDamageRequest->objectid))
    {
        if (object->m_pSyncer == nullptr || object->m_pSyncer == pNetworkPlayer)
        {
            return;
        }

        // only the syncer applies damage
        pObjectDamageRequest->senderPlayerid = pNetworkPlayer->m_playerId;
        GetPacketFactory().Send(*pObjectDamageRequest, object->m_pSyncer);
    }
}

PACKET_HANDLER(
    ePacketType::OBJECT_DAMAGE, Packets::Objects::ObjectDamage* pObjectDamage, CNetworkPlayer* pNetworkPlayer)
{
    if (auto object = CNetworkObjectManager::GetObject(pObjectDamage->objectid))
    {
        if (object->m_pSyncer == pNetworkPlayer)
        {
            object->m_damageState = pObjectDamage->state;
            GetPacketFactory().SendToAll(*pObjectDamage, pNetworkPlayer);
        }
        else
        {
            logger::warn("%s tried to damage someone else's object", pNetworkPlayer->GetName().c_str());
        }
    }
}
