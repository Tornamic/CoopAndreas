#include "stdafx.h"
#include "CNetworkObject.h"
#include <CObject.h>

CNetworkObject::CNetworkObject(const Packets::Objects::ObjectSpawn& packet)
{
    if (auto object = CNetworkObjectManager::GetObject(packet.objectid))
    {
        CNetworkObjectManager::Remove(object);
        delete object;
    }

    m_objectId = packet.objectid;
    m_bSyncing = false;
    m_nModelId = packet.modelid;

    if (!CreateObject(packet))
    {
        return;
    }
}

bool CNetworkObject::CreateObject(const Packets::Objects::ObjectSpawn& packet)
{
    int modelid = packet.modelid;
    if (modelid >= 20000 || CModelInfo::ms_modelInfoPtrs[modelid] == nullptr)
    {
        return false;
    }

    unsigned char oldFlags = CStreaming::ms_aInfoForModel[modelid].m_nFlags;
    CStreaming::RequestModel(modelid, GAME_REQUIRED);
    CStreaming::LoadAllRequestedModels(false);

    if (!(oldFlags & GAME_REQUIRED))
    {
        CStreaming::SetModelIsDeletable(modelid);
        CStreaming::SetModelTxdIsDeletable(modelid);
    }

    if (CStreaming::ms_aInfoForModel[modelid].m_nLoadState != LOADSTATE_LOADED)
    {
        return false;
    }

    CModelInfo::ms_modelInfoPtrs[modelid]->m_nAlpha = 255;

    // CObject::Create(int modelId, bool bUnused)
    m_pObject = plugin::CallAndReturn<CObject*, 0x5A1F60, int, bool>(modelid, false);

    if (m_pObject == nullptr)
    {
        return false;
    }

    m_pObject->RegisterReference((CEntity**)&m_pObject);

    m_pObject->m_nObjectType = OBJECT_MISSION;
    SetMatrix(m_pObject, packet.pos, packet.right, packet.up);

    if (CModelInfo::ms_modelInfoPtrs[modelid]->AsLodAtomicModelInfoPtr() != nullptr)
    {
        m_pObject->SetupBigBuilding();
    }

    m_pObject->m_nAreaCode = packet.areaCode;
    ApplyDamageState(m_pObject, packet.damageState);

    // not a hooked call site, so it doesn't become a hosted object
    CWorld::Add(m_pObject);

    return true;
}

CNetworkObject::~CNetworkObject()
{
    if (m_pObject != nullptr)
    {
        m_pObject->CleanUpOldReference((CEntity**)&m_pObject);
    }

    if (m_bSyncing)
    {
        Packets::Objects::ObjectRemove objectRemovePacket{};
        objectRemovePacket.objectid = m_objectId;
        GetPacketFactory().Send(objectRemovePacket);
    }
    else
    {
        if (m_pObject != nullptr && m_pObject->IsVTableValid())
        {
            CWorld::Remove(m_pObject);
            CWorld::RemoveReferencesToDeletedObject(m_pObject);
            delete m_pObject;
        }
    }
    m_pObject = nullptr;
}

bool CNetworkObject::HasChangedSinceLastUpdate()
{
    CMatrix* matrix = m_pObject->m_matrix;
    if (matrix == nullptr)
    {
        return false;
    }

    if (m_pObject->m_pAttachedTo != m_pLastAttachedTo)
    {
        return true;
    }

    if (m_pObject->m_pAttachedTo != nullptr)
    {
        const CQuaternion& rotation = m_pObject->m_qAttachedEntityRotation;
        if (DistanceBetweenPoints(m_pObject->m_vecAttachOffset, m_vecLastAttachOffset) > 0.001f ||
            DistanceBetweenPoints(m_pObject->m_vecAttachedEntityPosn, m_vecLastAttachTilt) > 0.001f ||
            DistanceBetweenPoints(rotation.imag, m_lastAttachRotation.imag) > 0.001f ||
            std::abs(rotation.real - m_lastAttachRotation.real) > 0.001f)
        {
            return true;
        }

        if (IsAttachedToNetworkEntity(m_pObject))
        {
            return false;
        }
    }

    return DistanceBetweenPoints(matrix->pos, m_vecLastPos) > 0.01f ||
           DistanceBetweenPoints(matrix->right, m_vecLastRight) > 0.001f ||
           DistanceBetweenPoints(matrix->up, m_vecLastUp) > 0.001f ||
           DistanceBetweenPoints(m_pObject->m_vecMoveSpeed, m_vecLastMoveSpeed) > 0.0005f ||
           DistanceBetweenPoints(m_pObject->m_vecTurnSpeed, m_vecLastTurnSpeed) > 0.0005f;
}

void CNetworkObject::StoreLastUpdate()
{
    CMatrix* matrix = m_pObject->m_matrix;
    if (matrix == nullptr)
    {
        return;
    }

    m_vecLastPos = matrix->pos;
    m_vecLastRight = matrix->right;
    m_vecLastUp = matrix->up;
    m_vecLastMoveSpeed = m_pObject->m_vecMoveSpeed;
    m_vecLastTurnSpeed = m_pObject->m_vecTurnSpeed;
    m_pLastAttachedTo = m_pObject->m_pAttachedTo;
    m_vecLastAttachOffset = m_pObject->m_vecAttachOffset;
    m_vecLastAttachTilt = m_pObject->m_vecAttachedEntityPosn;
    m_lastAttachRotation = m_pObject->m_qAttachedEntityRotation;
}

void CNetworkObject::GetAttachState(CObject* object, Packets::Objects::SObjectAttachState& state)
{
    state = Packets::Objects::SObjectAttachState();
    if (object->m_pAttachedTo == nullptr)
    {
        return;
    }

    state.bAttached = true;
    state.attachedTo.SetEntity(object->m_pAttachedTo);
    state.offset = object->m_vecAttachOffset;
    state.tilt = object->m_vecAttachedEntityPosn;
    state.rotationImag = object->m_qAttachedEntityRotation.imag;
    state.rotationReal = object->m_qAttachedEntityRotation.real;
}

bool CNetworkObject::IsAttachedToNetworkEntity(CObject* object)
{
    if (object->m_pAttachedTo == nullptr)
    {
        return false;
    }

    CNetworkEntitySerializer parent{};
    parent.SetEntity(object->m_pAttachedTo);
    return parent.GetEntity() == object->m_pAttachedTo;
}

bool CNetworkObject::ApplyAttachState(CObject* object, const Packets::Objects::SObjectAttachState& state)
{
    CNetworkEntitySerializer attachedTo = state.attachedTo;
    CEntity* parent = state.bAttached ? attachedTo.GetEntity() : nullptr;

    if (parent == nullptr || parent == object || parent->m_nType < ENTITY_TYPE_VEHICLE ||
        parent->m_nType > ENTITY_TYPE_OBJECT)
    {
        if (object->m_pAttachedTo != nullptr)
        {
            object->DettachEntityFromEntity(0.0f, 0.0f, 0.0f, false);
        }
        return false;
    }

    if (object->m_pAttachedTo != parent)
    {
        if (object->m_pAttachedTo != nullptr)
        {
            object->DettachEntityFromEntity(0.0f, 0.0f, 0.0f, false);
        }

        // CPhysical::AttachEntityToEntity(CPhysical*, CVector offset, CVector tilt), registers the reference
        plugin::CallMethodAndReturn<CPhysical*, 0x54D570, CPhysical*, CPhysical*, CVector, CVector>(
            object, (CPhysical*)parent, state.offset, state.tilt);
    }

    object->m_vecAttachOffset = state.offset;
    object->m_vecAttachedEntityPosn = state.tilt;
    object->m_qAttachedEntityRotation.imag = state.rotationImag;
    object->m_qAttachedEntityRotation.real = state.rotationReal;

    plugin::CallMethod<0x546FF0, CPhysical*>(object);  // CPhysical::PositionAttachedEntity
    return true;
}

CNetworkObject* CNetworkObject::CreateHosted(CObject* pObject)
{
    if (pObject->m_matrix == nullptr)
    {
        pObject->AllocateMatrix();
        pObject->m_placement.UpdateMatrix(pObject->m_matrix);
    }

    CNetworkObject* networkObject = new CNetworkObject();

    networkObject->m_pObject = pObject;
    pObject->RegisterReference((CEntity**)&networkObject->m_pObject);

    networkObject->m_objectId = CNetworkObjectManager::AllocateObjectId();
    networkObject->m_bSyncing = true;
    networkObject->m_nModelId = pObject->m_nModelIndex;
    networkObject->StoreLastUpdate();
    networkObject->m_nLastUpdateTime = GetTickCount();

    CNetworkObjectManager::Add(networkObject);

    Packets::Objects::ObjectSpawn objectSpawnPacket{};
    objectSpawnPacket.objectid = networkObject->m_objectId;
    objectSpawnPacket.modelid = pObject->m_nModelIndex;
    objectSpawnPacket.pos = pObject->m_matrix->pos;
    objectSpawnPacket.right = pObject->m_matrix->right;
    objectSpawnPacket.up = pObject->m_matrix->up;
    objectSpawnPacket.areaCode = pObject->m_nAreaCode;
    GetDamageState(pObject, objectSpawnPacket.damageState);
    GetPacketFactory().Send(objectSpawnPacket);

    return networkObject;
}

void CNetworkObject::SetMatrix(CObject* object, const CVector& pos, const CVector& right, const CVector& up)
{
    if (object->m_matrix == nullptr)
    {
        object->AllocateMatrix();
    }

    object->m_matrix->right = right;
    object->m_matrix->up = up;
    object->m_matrix->at.Cross(right, up);
    object->m_matrix->pos = pos;

    plugin::CallMethod<0x446F90, CEntity*>(object);  // CEntity::UpdateRW
    object->UpdateRwFrame();
}

void CNetworkObject::SendDamageIfChanged(const Packets::Objects::SObjectDamageState& oldState, float damage,
    CVector* colPos, CVector* colNormal, CEntity* damager, eWeaponType weapon)
{
    Packets::Objects::ObjectDamage packet{};
    GetDamageState(m_pObject, packet.state);

    if (packet.state == oldState)
    {
        return;
    }

    packet.objectid = m_objectId;
    packet.damage = damage;
    packet.weapon = (uint8_t)weapon;
    packet.bHasColPos = colPos != nullptr;
    if (colPos != nullptr)
    {
        packet.colPos = *colPos;
    }
    packet.bHasColNormal = colNormal != nullptr;
    if (colNormal != nullptr)
    {
        packet.colNormal = *colNormal;
    }
    packet.damager.SetEntity(damager);
    GetPacketFactory().Send(packet);
}

void CNetworkObject::CallObjectDamage(CObject* object, float damage, CVector* colPos, CVector* colNormal,
    CEntity* damager, eWeaponType weapon, bool bIgnoreOnlyDamagedByPlayer)
{
    bool oldApplying = ms_bApplyingNetworkDamage;
    bool oldOnlyDamagedByPlayer = object->m_nPhysicalFlags.bInvulnerable;  // bOnlyDamagedByPlayer (0x400000)

    if (bIgnoreOnlyDamagedByPlayer)
    {
        object->m_nPhysicalFlags.bInvulnerable = false;
    }

    ms_bApplyingNetworkDamage = true;
    plugin::CallMethod<0x5A0D90, CObject*, float, CVector*, CVector*, CEntity*, eWeaponType>(
        object, damage, colPos, colNormal, damager, weapon);
    ms_bApplyingNetworkDamage = oldApplying;

    object->m_nPhysicalFlags.bInvulnerable = oldOnlyDamagedByPlayer;
}

void CNetworkObject::GetDamageState(CObject* object, Packets::Objects::SObjectDamageState& state)
{
    state.health = object->m_fHealth;
    state.lastDamagedWeaponType = object->m_nLastWeaponDamage;
    state.bRenderDamaged = object->m_bRenderDamaged;
    state.bIsVisible = object->m_bIsVisible;
    state.bUsesCollision = object->m_bUsesCollision;
    state.bHasBeenShattered = object->m_nObjectFlags.bIsBroken;
    state.bHasExploded = object->m_nObjectFlags.bIsExploded;
}

void CNetworkObject::ApplyDamageState(CObject* object, const Packets::Objects::SObjectDamageState& state)
{
    if (object->m_bRenderDamaged != state.bRenderDamaged && object->m_pRwObject != nullptr)
    {
        object->DeleteRwObject();
    }

    object->m_fHealth = state.health;
    object->m_nLastWeaponDamage = state.lastDamagedWeaponType;
    object->m_bRenderDamaged = state.bRenderDamaged;
    object->m_bIsVisible = state.bIsVisible;
    object->m_bUsesCollision = state.bUsesCollision;
    object->m_nObjectFlags.bIsBroken = state.bHasBeenShattered;
    object->m_nObjectFlags.bIsExploded = state.bHasExploded;
}
