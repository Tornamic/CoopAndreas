#include "network/packets/objects.h"
#include "network/packet_types.h"
#include "stdafx.h"

PACKET_HANDLER(ePacketType::OBJECT_SPAWN, Packets::Objects::ObjectSpawn* pObjectSpawn)
{
#ifdef PACKET_DEBUG_MESSAGES
    CChat::AddMessage("OBJECT SPAWN %s %d %.1f %.1f %.1f", to_string(pObjectSpawn->objectid).c_str(),
        pObjectSpawn->modelid, pObjectSpawn->pos.x, pObjectSpawn->pos.y, pObjectSpawn->pos.z);
#endif

    CNetworkObject* pExisting = CNetworkObjectManager::GetObject(pObjectSpawn->objectid);
    if (pExisting != nullptr && pExisting->m_bSyncing)
    {
        return;
    }

    CNetworkObject* pNetworkObject = new CNetworkObject(*pObjectSpawn);
    CNetworkObjectManager::Add(pNetworkObject);
}

PACKET_HANDLER(ePacketType::OBJECT_REMOVE, Packets::Objects::ObjectRemove* pObjectRemove)
{
#ifdef PACKET_DEBUG_MESSAGES
    CChat::AddMessage("OBJECT REMOVE %s", to_string(pObjectRemove->objectid).c_str());
#endif
    CNetworkObject* pNetworkObject = CNetworkObjectManager::GetObject(pObjectRemove->objectid);
    if (pNetworkObject != nullptr && !pNetworkObject->m_bSyncing)
    {
        CNetworkObjectManager::Remove(pNetworkObject);
        delete pNetworkObject;
    }
}

PACKET_HANDLER(ePacketType::OBJECT_UPDATE, Packets::Objects::ObjectUpdate* pObjectUpdate)
{
    CNetworkObject* pNetworkObject = CNetworkObjectManager::GetObject(pObjectUpdate->objectid);

    if (pNetworkObject == nullptr || pNetworkObject->m_bSyncing)
    {
        return;
    }

    CObject* pObject = pNetworkObject->m_pObject;
    if (pObject == nullptr)
    {
        return;
    }

    if (CNetworkObject::ApplyAttachState(pObject, pObjectUpdate->attach))
    {
        return;
    }

    CWorld::Remove(pObject);
    CNetworkObject::SetMatrix(pObject, pObjectUpdate->pos, pObjectUpdate->right, pObjectUpdate->up);
    CWorld::Add(pObject);

    pObject->m_vecMoveSpeed = pObjectUpdate->moveSpeed;
    pObject->m_vecTurnSpeed = pObjectUpdate->turnSpeed;
}

PACKET_HANDLER(ePacketType::OBJECT_DAMAGE_REQUEST, Packets::Objects::ObjectDamageRequest* pObjectDamageRequest)
{
    CNetworkObject* pNetworkObject = CNetworkObjectManager::GetObject(pObjectDamageRequest->objectid);
    if (pNetworkObject == nullptr || !pNetworkObject->m_bSyncing || pNetworkObject->m_pObject == nullptr)
    {
        return;
    }

    CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(pObjectDamageRequest->senderPlayerid);
    if (pNetworkPlayer == nullptr || pNetworkPlayer->m_pPed == nullptr)
    {
        return;
    }

    CEntity* pDamager = pNetworkPlayer->m_pPed;
    if (pObjectDamageRequest->bInVehicle && pNetworkPlayer->m_pPed->m_pVehicle != nullptr)
    {
        pDamager = pNetworkPlayer->m_pPed->m_pVehicle;
    }

    CObject* pObject = pNetworkObject->m_pObject;
    CVector colPos = pObjectDamageRequest->bHasColPos ? (CVector)pObjectDamageRequest->colPos : pObject->GetPosition();
    CVector colNormal =
        pObjectDamageRequest->bHasColNormal ? pObjectDamageRequest->colNormal : CVector(0.0f, 0.0f, 1.0f);
    eWeaponType weapon = (eWeaponType)pObjectDamageRequest->weapon;

    Packets::Objects::SObjectDamageState oldState{};
    CNetworkObject::GetDamageState(pObject, oldState);

    // the request comes from a player, so `bOnlyDamagedByPlayer` is satisfied
    CNetworkObject::CallObjectDamage(
        pObject, pObjectDamageRequest->damage, &colPos, &colNormal, pDamager, weapon, true);

    if (pNetworkObject->m_pObject != nullptr)
    {
        pNetworkObject->SendDamageIfChanged(
            oldState, pObjectDamageRequest->damage, &colPos, &colNormal, pDamager, weapon);
    }
}

PACKET_HANDLER(ePacketType::OBJECT_DAMAGE, Packets::Objects::ObjectDamage* pObjectDamage)
{
    CNetworkObject* pNetworkObject = CNetworkObjectManager::GetObject(pObjectDamage->objectid);
    if (pNetworkObject == nullptr || pNetworkObject->m_bSyncing || pNetworkObject->m_pObject == nullptr)
    {
        return;
    }

    CObject* pObject = pNetworkObject->m_pObject;

    if (pObject->m_bIsVisible && pObject->m_pRwObject != nullptr)
    {
        CVector colPos = pObjectDamage->bHasColPos ? (CVector)pObjectDamage->colPos : pObject->GetPosition();
        CVector colNormal = pObjectDamage->bHasColNormal ? pObjectDamage->colNormal : CVector(0.0f, 0.0f, 1.0f);

        CNetworkObject::CallObjectDamage(pObject, pObjectDamage->damage, &colPos, &colNormal,
            pObjectDamage->damager.GetEntity(), (eWeaponType)pObjectDamage->weapon, true);
    }

    if (pNetworkObject->m_pObject != nullptr)
    {
        CNetworkObject::ApplyDamageState(pNetworkObject->m_pObject, pObjectDamage->state);
    }
}
