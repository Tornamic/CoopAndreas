#include "stdafx.h"
#include "CWorldObjectSync.h"
#include "CNetworkPlayerManager.h"
#include "CNetworkVehicleManager.h"
#include "CNetworkPedManager.h"
#include <CDummyObject.h>
#include <CPools.h>

using namespace Packets::World;

std::vector<CWorldObjectSync::SSlot> CWorldObjectSync::ms_slots;
CWorldObjectSync::KeyMap CWorldObjectSync::ms_slotByKey;
CWorldObjectSync::StateMap CWorldObjectSync::ms_states;

constexpr uint8_t OBJECT_GAME = 1;  // eObjectType: made from a map dummy

static CVector Cross(const CVector& a, const CVector& b)
{
    return CVector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

static float Dot(const CVector& a, const CVector& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static WorldObjectKey MakeKey(CDummyObject* pDummy)
{
    const CVector& pos = pDummy->GetPosition();
    WorldObjectKey key{};
    key.model = static_cast<int16_t>(pDummy->m_nModelIndex);
    key.x = static_cast<int32_t>(std::lround(pos.x * 100.0f));
    key.y = static_cast<int32_t>(std::lround(pos.y * 100.0f));
    key.z = static_cast<int32_t>(std::lround(pos.z * 100.0f));
    return key;
}

// the dummy's own placement: GetMatrix() would allocate a matrix for it
static WorldObjectTransform GetDummyTransform(CDummyObject* pDummy)
{
    WorldObjectTransform transform{};
    transform.pos = pDummy->GetPosition();
    if (pDummy->m_matrix)
    {
        transform.right = pDummy->m_matrix->right;
        transform.forward = pDummy->m_matrix->up;
    }
    else
    {
        float heading = pDummy->m_placement.m_fHeading;
        transform.right = CVector(std::cos(heading), std::sin(heading), 0.0f);
        transform.forward = CVector(-std::sin(heading), std::cos(heading), 0.0f);
    }
    return transform;
}

static WorldObjectTransform GetTransform(CObject* pObject)
{
    WorldObjectTransform transform{};
    transform.pos = pObject->GetPosition();
    transform.right = pObject->m_matrix->right;
    transform.forward = pObject->m_matrix->up;
    return transform;
}

static bool IsDisplaced(const WorldObjectTransform& transform, const WorldObjectTransform& original)
{
    CVector delta = transform.pos - original.pos;
    return Dot(delta, delta) > 0.05f * 0.05f || Dot(transform.right, original.right) < 0.999f ||
           Dot(transform.forward, original.forward) < 0.999f;
}

static uint8_t CollectDamage(CObject* pObject)
{
    uint8_t flags = 0;
    if (pObject->m_bRenderDamaged)
    {
        flags |= WorldObjectState::DAMAGED;
    }
    if (!pObject->m_bUsesCollision && !pObject->m_bIsVisible)
    {
        flags |= WorldObjectState::SMASHED;
    }
    if (pObject->m_nObjectFlags.bIsBroken)
    {
        flags |= WorldObjectState::BROKEN;
    }
    if (pObject->m_nObjectFlags.bIsExploded)
    {
        flags |= WorldObjectState::EXPLODED;
    }
    return flags;
}

static void SetTransform(CObject* pObject, const WorldObjectTransform& transform, bool bStatic)
{
    // like CObject::Teleport, with the rotation: the sectors and the moving list follow the new matrix
    CWorld::Remove(pObject);
    CMatrix& matrix = *pObject->m_matrix;
    matrix.right = transform.right;
    matrix.up = transform.forward;
    matrix.at = Cross(transform.right, transform.forward);
    matrix.pos = transform.pos;
    pObject->SetIsStatic(bStatic);
    if (bStatic)
    {
        pObject->m_vecMoveSpeed = CVector();
        pObject->m_vecTurnSpeed = CVector();
    }
    CWorld::Add(pObject);
}

// the same CObject::ObjectDamage the other player had: the effects, the pieces, the explosion of a petrol pump
static void ReplayDamage(CObject* pObject)
{
    auto& flags = pObject->m_nPhysicalFlags;
    auto saved = flags;
    flags.bBulletProof = flags.bFireProof = flags.bCollisionProof = flags.bMeleeProof = flags.bInvulnerable =
        flags.bExplosionProof = false;

    CVector origin = pObject->GetPosition();
    CVector direction(0.0f, 0.0f, 1.0f);
    constexpr auto WEAPON_RAMMEDBYCAR = static_cast<eWeaponType>(49);  // not in the plugin-sdk enum
    pObject->ObjectDamage(10000.0f, &origin, &direction, FindPlayerPed(0), WEAPON_RAMMEDBYCAR);

    // ObjectDamage sets some of them itself (explosion proof after a smash)
    flags.bBulletProof |= saved.bBulletProof;
    flags.bFireProof |= saved.bFireProof;
    flags.bCollisionProof |= saved.bCollisionProof;
    flags.bMeleeProof |= saved.bMeleeProof;
    flags.bInvulnerable |= saved.bInvulnerable;
    flags.bExplosionProof |= saved.bExplosionProof;
}

// the end state of CObject::ObjectDamage without the effects: an object made just now from its dummy
static void ForceDamage(CObject* pObject, uint8_t flags)
{
    if ((flags & WorldObjectState::DAMAGED) && !pObject->m_bRenderDamaged)
    {
        // the renderer makes the damaged model, as after COL_DAMAGE_EFFECT_CHANGE_MODEL
        pObject->DeleteRwObject();
        pObject->m_bRenderDamaged = true;
    }

    if ((flags & WorldObjectState::SMASHED) && (pObject->m_bUsesCollision || pObject->m_bIsVisible))
    {
        pObject->m_bUsesCollision = false;
        pObject->m_bIsVisible = false;
        if (!pObject->m_bIsStatic)
        {
            pObject->RemoveFromMovingList();
        }
        pObject->SetIsStatic(true);
        pObject->m_nPhysicalFlags.bExplosionProof = true;
        pObject->m_vecMoveSpeed = CVector();
        pObject->m_vecTurnSpeed = CVector();
        pObject->DeleteRwObject();
        pObject->m_fHealth = 0.0f;
    }

    if (flags & WorldObjectState::BROKEN)
    {
        pObject->m_nObjectFlags.bIsBroken = true;
    }
    if (flags & WorldObjectState::EXPLODED)
    {
        pObject->m_nObjectFlags.bIsExploded = true;
    }
}

void CWorldObjectSync::Reset()
{
    ms_slots.clear();
    ms_slotByKey.clear();
    ms_states.clear();
}

CWorldObjectSync::SSlot* CWorldObjectSync::FindSlot(const WorldObjectKey& key)
{
    auto it = ms_slotByKey.find(key);
    return it != ms_slotByKey.end() ? &ms_slots[it->second] : nullptr;
}

void CWorldObjectSync::Remember(const WorldObjectKey& key, bool bTransform, const WorldObjectState& state)
{
    WorldObjectState& known = ms_states[key];
    known.flags |= state.flags & WorldObjectState::DAMAGE_MASK;
    if (bTransform)
    {
        known.flags = (known.flags & ~WorldObjectState::MOVED) | (state.flags & WorldObjectState::MOVED);
        known.transform = state.transform;
    }

    if (known.flags == 0)
    {
        ms_states.erase(key);
    }
}

void CWorldObjectSync::SendState(const SSlot& slot, bool bTransform, uint8_t flags)
{
    WorldObjectStates packet{};
    packet.live = true;
    packet.count = 1;
    auto& entry = packet.entries[0];
    entry.key = slot.key;
    entry.hasTransform = bTransform;
    entry.state.flags = flags;
    entry.state.transform = slot.lastTransform;
    GetPacketFactory().Send(packet);
    logger::info("world objects: sent %d at %d %d %d, flags %d%s", slot.key.model, slot.key.x, slot.key.y,
        slot.key.z, flags, bTransform ? ", where it stopped" : "");

    Remember(slot.key, bTransform, entry.state);
}

void CWorldObjectSync::Apply(SSlot& slot, const WorldObjectState& state, bool bLive, bool bTransform)
{
    CObject* pObject = slot.pObject;
    uint8_t target = state.flags & WorldObjectState::DAMAGE_MASK;
    logger::info("world objects: %s %d at %d %d %d, flags %d", bLive ? "live" : "applied", slot.key.model, slot.key.x,
        slot.key.y, slot.key.z, state.flags);
    constexpr uint8_t BY_DAMAGE = WorldObjectState::DAMAGED | WorldObjectState::SMASHED | WorldObjectState::BROKEN;

    if (bLive)
    {
        // COL_DAMAGE_EFFECT_CHANGE_THEN_SMASH takes two hits
        for (int i = 0; i < 2 && (target & ~CollectDamage(pObject) & BY_DAMAGE); i++)
        {
            ReplayDamage(pObject);
        }

        if ((target & WorldObjectState::EXPLODED) && !pObject->m_nObjectFlags.bIsExploded)
        {
            pObject->TryToExplode();
        }
    }
    ForceDamage(pObject, target);

    if (bTransform)
    {
        if (state.flags & WorldObjectState::MOVED)
        {
            SetTransform(pObject, state.transform, true);
        }
        else if (IsDisplaced(GetTransform(pObject), slot.original))
        {
            SetTransform(pObject, slot.original, true);
        }
    }

    slot.flags = CollectDamage(pObject);
    // a damage-only state leaves the motion alone: whoever moves the object still sends where it stops
    if (bTransform)
    {
        slot.bMoving = !pObject->m_bIsStatic;
        slot.bMover = false;
        slot.remoteUntil = GetTickCount() + REMOTE_REST_TIME;
    }
}

bool CWorldObjectSync::ShouldBecomeMover(CObject* pObject)
{
    CPlayerPed* pLocalPed = FindPlayerPed(0);
    if (pLocalPed == nullptr)
    {
        return false;
    }

    // who hit it (only compared, it may point to a deleted entity). A clone hits the object here too, its owner
    // sends the motion
    CEntity* pDamager = pObject->m_pDamageEntity;
    if (pDamager != nullptr)
    {
        if (pDamager == pLocalPed || (pLocalPed->m_nPedFlags.bInVehicle && pDamager == pLocalPed->m_pVehicle))
        {
            return true;
        }
        if (CNetworkVehicle* pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pDamager))
        {
            return pNetworkVehicle->m_bSyncing;
        }
        if (CNetworkPed* pNetworkPed = CNetworkPedManager::GetPed(pDamager))
        {
            return pNetworkPed->m_bSyncing;
        }
        if (CNetworkPlayerManager::GetPlayer(pDamager) != nullptr)
        {
            return false;
        }
    }

    // an explosion, another object, a thrown thing: it moves here, maybe only here. If it moves for another player
    // too, the lower player id keeps it (HandlePacket)
    return true;
}

void CWorldObjectSync::OnObjectAppeared(SSlot& slot)
{
    CObject* pObject = slot.pObject;
    slot.key = MakeKey(pObject->m_pDummyObject);
    slot.original = GetDummyTransform(pObject->m_pDummyObject);
    slot.flags = CollectDamage(pObject);
    slot.bMoving = !pObject->m_bIsStatic;
    slot.bMover = false;
    slot.remoteUntil = 0;
    ms_slotByKey[slot.key] = static_cast<int>(&slot - ms_slots.data());

    // made from its dummy this frame, before it is rendered: as the others see it
    auto it = ms_states.find(slot.key);
    if (it != ms_states.end())
    {
        Apply(slot, it->second, false, true);
    }
}

void CWorldObjectSync::OnObjectGone(SSlot& slot)
{
    // turned back into its dummy on the way: where we saw it last is where it stays
    if (slot.bMover)
    {
        uint8_t flags = slot.flags;
        if (IsDisplaced(slot.lastTransform, slot.original))
        {
            flags |= WorldObjectState::MOVED;
        }
        SendState(slot, true, flags);
    }

    auto it = ms_slotByKey.find(slot.key);
    if (it != ms_slotByKey.end() && &ms_slots[it->second] == &slot)
    {
        ms_slotByKey.erase(it);
    }
    slot = SSlot{};
}

void CWorldObjectSync::ProcessSlot(SSlot& slot, uint32_t now)
{
    CObject* pObject = slot.pObject;

    uint8_t flags = CollectDamage(pObject);
    if (flags != slot.flags)
    {
        // anyone may report it: the server only adds the damage up
        slot.flags = flags;
        SendState(slot, false, flags);
    }

    bool bMoving = !pObject->m_bIsStatic;
    bool bWasMoving = slot.bMoving;
    slot.bMoving = bMoving;

    if (now < slot.remoteUntil)
    {
        return;  // our physics plays the motion the mover sent
    }

    if (bMoving && !bWasMoving)
    {
        // doors swing back, buoys float, held boxes go with their holder
        bool bOwnMotion = pObject->m_nPhysicalFlags.bDisableMoveForce || pObject->m_nPhysicalFlags.bTouchingWater ||
                          pObject->m_nPhysicalFlags.bAttachedToEntity || pObject->m_pAttachedTo != nullptr;
        slot.bMover = !bOwnMotion && ShouldBecomeMover(pObject);
        slot.lastMotionTime = 0;
        if (slot.bMover)
        {
            logger::info("world objects: we move %d at %d %d %d", slot.key.model, slot.key.x, slot.key.y, slot.key.z);
        }
    }

    if (!slot.bMover)
    {
        return;
    }

    slot.lastTransform = GetTransform(pObject);

    if (bMoving)
    {
        if (now - slot.lastMotionTime >= MOTION_INTERVAL)
        {
            slot.lastMotionTime = now;

            WorldObjectMotion packet{};
            packet.key = slot.key;
            packet.transform = slot.lastTransform;
            packet.moveSpeed = pObject->m_vecMoveSpeed;
            packet.turnSpeed = pObject->m_vecTurnSpeed;
            GetPacketFactory().Send(packet);
        }
        return;
    }

    // came to rest
    slot.bMover = false;
    uint8_t state = slot.flags;
    if (IsDisplaced(slot.lastTransform, slot.original))
    {
        state |= WorldObjectState::MOVED;
    }
    SendState(slot, true, state);
}

void CWorldObjectSync::Process()
{
    auto* pPool = CPools::ms_pObjectPool;
    if (pPool == nullptr)
    {
        return;
    }

    if (ms_slots.size() != static_cast<size_t>(pPool->m_nSize))
    {
        ms_slots.assign(pPool->m_nSize, SSlot{});
        ms_slotByKey.clear();
    }

    uint32_t now = GetTickCount();
    for (int i = 0; i < pPool->m_nSize; i++)
    {
        SSlot& slot = ms_slots[i];
        CObject* pObject = pPool->GetAt(i);

        // OBJECT_GAME with its dummy: a map object. Mission objects are the scripts', pieces and pickups are local
        bool bMapObject = pObject != nullptr && pObject->m_nObjectType == OBJECT_GAME && pObject->m_pDummyObject &&
                          pObject->m_matrix;
        int ref = bMapObject ? CPools::GetObjectRef(pObject) + 1 : 0;

        if (slot.ref != 0 && slot.ref != ref)
        {
            OnObjectGone(slot);
        }

        if (!bMapObject)
        {
            continue;
        }

        if (slot.ref == 0)
        {
            slot.ref = ref;
            slot.pObject = pObject;
            OnObjectAppeared(slot);
            continue;
        }

        ProcessSlot(slot, now);
    }
}

void CWorldObjectSync::HandlePacket(WorldObjectStates* pPacket)
{
    for (int i = 0; i < pPacket->count; i++)
    {
        const auto& entry = pPacket->entries[i];
        if (entry.state.flags == 0)
        {
            ms_states.erase(entry.key);
        }
        else
        {
            Remember(entry.key, entry.hasTransform, entry.state);
        }

        SSlot* pSlot = FindSlot(entry.key);
        if (pSlot == nullptr)
        {
            continue;  // still a dummy here, gets the state when it becomes an object
        }

        if (entry.state.flags == 0)
        {
            // forgotten (nobody was near) - nothing to undo on a living object; pushed back - put it back
            if (pPacket->live)
            {
                Apply(*pSlot, entry.state, false, true);
            }
            continue;
        }

        Apply(*pSlot, entry.state, pPacket->live, entry.hasTransform);
    }
}

void CWorldObjectSync::HandlePacket(WorldObjectMotion* pPacket)
{
    SSlot* pSlot = FindSlot(pPacket->key);
    if (pSlot == nullptr || !pSlot->pObject->m_bUsesCollision)
    {
        return;
    }

    // both of us started it: the lower id keeps it
    if (pSlot->bMover && CNetworkPlayerManager::m_localPlayerId < pPacket->mover)
    {
        return;
    }
    pSlot->bMover = false;

    CObject* pObject = pSlot->pObject;
    SetTransform(pObject, pPacket->transform, false);
    pObject->m_vecMoveSpeed = pPacket->moveSpeed;
    pObject->m_vecTurnSpeed = pPacket->turnSpeed;

    pSlot->bMoving = true;
    pSlot->remoteUntil = GetTickCount() + REMOTE_MOTION_TIME;
}
