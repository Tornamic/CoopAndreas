#include "stdafx.h"
#include "DamageHooks.h"

bool __fastcall CEventDamage__AffectsPed_Hook(CEventDamage* This, SKIP_EDX, CPed* pPed)
{
    if (CLocalPlayer::GetIsHostingEntity(pPed))
    {
        return plugin::CallMethodAndReturn<bool, 0x4B35A0>(This, pPed);
    }
    return false;
}

static bool IsNetworkPlayerOrTheirVehicle(CEntity* entity)
{
    if (entity == nullptr)
    {
        return false;
    }

    if (entity->m_nType == ENTITY_TYPE_PED)
    {
        return CNetworkPlayerManager::GetPlayer(entity) != nullptr;
    }

    if (entity->m_nType == ENTITY_TYPE_VEHICLE)
    {
        CPed* driver = ((CVehicle*)entity)->m_pDriver;
        return driver != nullptr && CNetworkPlayerManager::GetPlayer(driver) != nullptr;
    }

    return false;
}

// the syncer of a network object is the only PC that applies damage on its own,
// everyone else asks it (`ObjectDamageRequest`) or replays its result (`ObjectDamage`)
static void __fastcall CObject__ObjectDamage_Hook(
    CObject* This, SKIP_EDX, float damage, CVector* colPos, CVector* colNormal, CEntity* damager, eWeaponType weapon)
{
    CNetworkObject* networkObject = CNetworkObjectManager::GetObject(This);

    if (CNetworkObject::ms_bApplyingNetworkDamage || !CNetwork::m_bAuthenticated || networkObject == nullptr)
    {
        plugin::CallMethod<0x5A0D90, CObject*, float, CVector*, CVector*, CEntity*, eWeaponType>(
            This, damage, colPos, colNormal, damager, weapon);
        return;
    }

    if (networkObject->m_bSyncing)
    {

        if (IsNetworkPlayerOrTheirVehicle(damager))
        {
            return;
        }

        Packets::Objects::SObjectDamageState oldState{};
        CNetworkObject::GetDamageState(This, oldState);
        CNetworkObject::CallObjectDamage(This, damage, colPos, colNormal, damager, weapon, false);
        networkObject->SendDamageIfChanged(oldState, damage, colPos, colNormal, damager, weapon);
        return;
    }

    bool bInVehicle = damager != nullptr && damager == FindPlayerVehicle(0, false);
    if (damager == nullptr || (damager != FindPlayerPed(0) && !bInVehicle))
    {
        return;
    }

    if (!This->m_bUsesCollision)
    {
        return;
    }

    if (weapon == 55 && damager->m_nType == ENTITY_TYPE_VEHICLE)  // WEAPONTYPE_UNIDENTIFIED
        weapon = (eWeaponType)50;                                 // WEAPONTYPE_RUNOVERBYCAR

    if (!This->CanPhysicalBeDamaged(weapon, nullptr))
    {
        return;
    }

    if (damage * This->m_pObjectInfo->m_fColDamageMultiplier < 0.01f)
    {
        return;
    }

    Packets::Objects::ObjectDamageRequest packet{};
    packet.objectid = networkObject->m_objectId;
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
    packet.bInVehicle = bInVehicle;
    GetPacketFactory().Send(packet);
}

void DamageHooks::InjectHooks()
{
    patch::RedirectCall(0x548DC2, CObject__ObjectDamage_Hook);  // CPhysical::ApplyCollision
    patch::RedirectCall(0x54A6D4, CObject__ObjectDamage_Hook);  // CPhysical::ApplySoftCollision
    patch::RedirectCall(0x54C69B, CObject__ObjectDamage_Hook);  // CPhysical::ProcessCollisionSectorList
    patch::RedirectCall(0x54C791, CObject__ObjectDamage_Hook);  // CPhysical::ProcessCollisionSectorList
    patch::RedirectCall(0x54C7EC, CObject__ObjectDamage_Hook);  // CPhysical::ProcessCollisionSectorList
    patch::RedirectCall(0x54C8E4, CObject__ObjectDamage_Hook);  // CPhysical::ProcessCollisionSectorList
    patch::RedirectCall(0x54CEE0, CObject__ObjectDamage_Hook);  // CPhysical::ProcessCollisionSectorList
    patch::RedirectCall(0x5679AD, CObject__ObjectDamage_Hook);  // CWorld::TriggerExplosionSectorList
    patch::RedirectCall(0x56846E, CObject__ObjectDamage_Hook);  // CWorld::TriggerExplosionSectorList
    patch::RedirectCall(0x5A1489, CObject__ObjectDamage_Hook);  // CObject::Explode
    patch::RedirectCall(0x5A223B, CObject__ObjectDamage_Hook);  // CObject::ProcessControl
    patch::RedirectCall(0x61D578, CObject__ObjectDamage_Hook);  // CTaskSimpleFight::FightHitObj
    patch::RedirectCall(0x6A99B7, CObject__ObjectDamage_Hook);  // CAutomobile::ProcessHarvester
    patch::RedirectCall(0x7365C6, CObject__ObjectDamage_Hook);  // CBulletInfo::Update
    patch::RedirectCall(0x7365E3, CObject__ObjectDamage_Hook);  // CBulletInfo::Update
    patch::RedirectCall(0x736692, CObject__ObjectDamage_Hook);  // CBulletInfo::Update
    patch::RedirectCall(0x73BBB6, CObject__ObjectDamage_Hook);  // CWeapon::DoBulletImpact
    patch::RedirectCall(0x73BBE1, CObject__ObjectDamage_Hook);  // CWeapon::DoBulletImpact
    patch::RedirectCall(0x73BBFF, CObject__ObjectDamage_Hook);  // CWeapon::DoBulletImpact
    patch::RedirectCall(0x73BD20, CObject__ObjectDamage_Hook);  // CWeapon::DoBulletImpact

    /* patch::RedirectCall(0x439055, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x4813CD, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x4BA06F, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x5680B8, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x568271, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x5E2F57, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x5E3020, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x5F0CBB, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x5F0F25, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x6049B1, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x604AA0, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x6297FB, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x65401F, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x656974, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x67C8FC, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x686033, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x68B44E, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x6A929D, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x6A94DF, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x6B6378, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x6B6566, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x6D83F5, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x6DB30B, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x6DBE7E, CEventDamage__AffectsPed_Hook);
     patch::RedirectCall(0x73A687, CEventDamage__AffectsPed_Hook);
     patch::SetPointer(0x85AD8C, CEventDamage__AffectsPed_Hook);*/
}