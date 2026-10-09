#include "stdafx.h"
#include "CPickupSync.h"
#include <CPickups.h>
#include <cmath>

namespace
{
    bool s_bApplyingRemote = false;
    unsigned int s_lastNotifiedTick = 0;
    CVector s_lastNotifiedPos{};
    CVector s_lastJetpackDropPos{};
    bool s_hasLastJetpackDropPos = false;

    constexpr float PICKUP_MATCH_RADIUS = 3.0f;

    bool IsCollectedPickupType(ePickupType pickupType)
    {
        switch (pickupType)
        {
            case PICKUP_ONCE:
            case PICKUP_ONCE_TIMEOUT:
            case PICKUP_ONCE_TIMEOUT_SLOW:
            case PICKUP_COLLECTABLE1:
            case PICKUP_SNAPSHOT:
            case PICKUP_ONCE_FOR_MISSION:
                return true;
            default:
                return false;
        }
    }

    bool ShouldTrackCollectedState(ePickupType pickupType)
    {
        return IsCollectedPickupType(pickupType) || pickupType == PICKUP_MONEY
            || pickupType == PICKUP_MONEY_DOESNTDISAPPEAR;
    }

    void DisableRespawningPickup(CPickup& pickup)
    {
        const auto pickupType = static_cast<ePickupType>(pickup.m_nPickupType);
        uint32_t regenDelay = pickupType == PICKUP_ON_STREET_SLOW ? 36000u : 30000u;

        pickup.m_nRegenerationTime = CTimer::m_snTimeInMilliseconds + regenDelay;
        pickup.m_nFlags.bDisabled = true;
        pickup.GetRidOfObjects();
    }

    uint16_t GetModelIdForSlot(uint16_t pickupSlot)
    {
        if (pickupSlot >= MAX_NUM_PICKUPS)
        {
            return 0;
        }

        const CPickup& pickup = CPickups::aPickUps[pickupSlot];
        if (pickup.m_nPickupType == PICKUP_NONE || pickup.m_nModelIndex < 0)
        {
            return 0;
        }

        return static_cast<uint16_t>(pickup.m_nModelIndex);
    }

    void SendPickupCollected(uint16_t pickupSlot, const CVector& pos, uint16_t modelId)
    {
        if (!CNetwork::m_bAuthenticated || s_bApplyingRemote || modelId == 0)
        {
            return;
        }

        const unsigned int tickCount = GetTickCount();
        if ((pos - s_lastNotifiedPos).Magnitude() < 1.0f && tickCount < s_lastNotifiedTick + 100)
        {
            return;
        }

        s_lastNotifiedPos = pos;
        s_lastNotifiedTick = tickCount;

        Packets::World::PickupCollected packet{};
        packet.pickupSlot = pickupSlot;
        packet.pos_x = static_cast<int16_t>(std::floor(pos.x));
        packet.pos_y = static_cast<int16_t>(std::floor(pos.y));
        packet.pos_z = static_cast<int16_t>(std::floor(pos.z));
        packet.modelId = modelId;
        GetPacketFactory().Send(packet);
    }

    int FindPickupSlotNear(const CVector& target, float radius, short modelFilter = -1)
    {
        int bestSlot = -1;
        float bestDistance = radius;

        for (int i = 0; i < static_cast<int>(MAX_NUM_PICKUPS); ++i)
        {
            const CPickup& pickup = CPickups::aPickUps[i];
            if (pickup.m_nPickupType == PICKUP_NONE)
            {
                continue;
            }

            if (modelFilter >= 0 && pickup.m_nModelIndex != modelFilter)
            {
                continue;
            }

            CVector pos = CPickups::aPickUps[i].GetPosn();
            const float distance = (pos - target).Magnitude();
            if (distance < bestDistance)
            {
                bestDistance = distance;
                bestSlot = i;
            }
        }

        return bestSlot;
    }

    CVector ResolveSpawnPos(const CVector& position, short modelFilter = -1)
    {
        const int foundSlot = FindPickupSlotNear(position, PICKUP_MATCH_RADIUS, modelFilter);
        if (foundSlot < 0)
        {
            return position;
        }

        return CPickups::aPickUps[foundSlot].GetPosn();
    }

    void ApplyCollectibleReward(const CPickup& pickup)
    {
        const auto pickupType = static_cast<ePickupType>(pickup.m_nPickupType);
        if (pickupType != PICKUP_COLLECTABLE1 && pickupType != PICKUP_SNAPSHOT)
        {
            return;
        }

        if (pickup.m_nModelIndex < 0)
        {
            return;
        }

        CPickups::GivePlayerGoodiesWithPickUpMI(static_cast<unsigned short>(pickup.m_nModelIndex), 0);
    }

    void ApplyCollectedToSlot(uint16_t pickupSlot, bool applyCollectibleReward)
    {
        if (pickupSlot >= MAX_NUM_PICKUPS)
        {
            return;
        }

        CPickup& pickup = CPickups::aPickUps[pickupSlot];
        if (pickup.m_nPickupType == PICKUP_NONE)
        {
            return;
        }

        if (CPickups::IsPickUpPickedUp(CPickups::GetUniquePickupIndex(pickupSlot)))
        {
            return;
        }

        s_bApplyingRemote = true;

        if (applyCollectibleReward)
        {
            ApplyCollectibleReward(pickup);
        }

        const auto pickupType = static_cast<ePickupType>(pickup.m_nPickupType);

        if (pickupType == PICKUP_ON_STREET || pickupType == PICKUP_ON_STREET_SLOW)
        {
            DisableRespawningPickup(pickup);
            CPickups::AddToCollectedPickupsArray(pickupSlot);
        }
        else
        {
            pickup.GetRidOfObjects();

            if (ShouldTrackCollectedState(pickupType))
            {
                CPickups::AddToCollectedPickupsArray(pickupSlot);
                pickup.Remove();
            }
            else if (pickup.m_nModelIndex != -1)
            {
                pickup.m_nFlags.bDisabled = true;
                CPickups::AddToCollectedPickupsArray(pickupSlot);
            }
            else
            {
                CPickups::RemovePickUp(CPickups::GetUniquePickupIndex(pickupSlot));
            }
        }

        s_bApplyingRemote = false;
    }

    void SendPickupSpawn(const CVector& position, uint16_t modelId, uint8_t pickupType, uint16_t ammo, bool isEmpty)
    {
        Packets::World::PickupSpawn packet{};
        packet.pos = position;
        packet.modelId = modelId;
        packet.pickupType = pickupType;
        packet.ammo = ammo;
        packet.isEmpty = isEmpty;
        GetPacketFactory().Send(packet);
    }
}

bool CPickupSync::ShouldSyncSpawnType(uint8_t pickupType)
{
    switch (static_cast<ePickupType>(pickupType))
    {
        case PICKUP_IN_SHOP:
        case PICKUP_IN_SHOP_OUT_OF_STOCK:
        case PICKUP_PROPERTY_LOCKED:
        case PICKUP_PROPERTY_FORSALE:
        case PICKUP_ASSET_REVENUE:
            return false;
        default:
            return true;
    }
}

void CPickupSync::NotifyCollected(uint16_t pickupSlot, const CVector* posOverride)
{
    CVector pos{};
    if (posOverride != nullptr)
    {
        pos = *posOverride;
    }
    else if (pickupSlot < MAX_NUM_PICKUPS && CPickups::aPickUps[pickupSlot].m_nPickupType != PICKUP_NONE)
    {
        pos = CPickups::aPickUps[pickupSlot].GetPosn();
    }
    else
    {
        return;
    }

    SendPickupCollected(pickupSlot, pos, GetModelIdForSlot(pickupSlot));
}

void CPickupSync::NotifyCollectedAt(const CVector& pos, uint16_t modelId)
{
    SendPickupCollected(0, pos, modelId);
}

void CPickupSync::ApplyCollected(const Packets::World::PickupCollected& packet, bool applyCollectibleReward)
{
    const CVector targetPos(
        static_cast<float>(packet.pos_x), static_cast<float>(packet.pos_y), static_cast<float>(packet.pos_z));

    int foundSlot = -1;
    if (packet.modelId != 0)
    {
        foundSlot = FindPickupSlotNear(targetPos, PICKUP_MATCH_RADIUS, static_cast<short>(packet.modelId));
    }

    if (foundSlot < 0)
    {
        foundSlot = FindPickupSlotNear(targetPos, PICKUP_MATCH_RADIUS);
    }

    uint16_t pickupSlot = packet.pickupSlot;
    if (foundSlot >= 0)
    {
        pickupSlot = static_cast<uint16_t>(foundSlot);
    }
    else if (pickupSlot >= MAX_NUM_PICKUPS || CPickups::aPickUps[pickupSlot].m_nPickupType == PICKUP_NONE)
    {
        return;
    }

    ApplyCollectedToSlot(pickupSlot, applyCollectibleReward);
}

void CPickupSync::NotifyPickupSpawned(
    const CVector& position, uint16_t modelId, uint8_t pickupType, uint16_t ammo, bool isEmpty)
{
    if (!CNetwork::m_bAuthenticated || s_bApplyingRemote || !ShouldSyncSpawnType(pickupType))
    {
        return;
    }

    const CVector spawnPos = ResolveSpawnPos(position, static_cast<short>(modelId));

    if (modelId == MODEL_JETPACK)
    {
        s_lastJetpackDropPos = spawnPos;
        s_hasLastJetpackDropPos = true;
    }

    SendPickupSpawn(spawnPos, modelId, pickupType, ammo, isEmpty);
}

void CPickupSync::NotifyMoneySpawned(const CVector& position, int amount)
{
    if (!CNetwork::m_bAuthenticated || s_bApplyingRemote || amount <= 0)
    {
        return;
    }

    CVector spawnPos = position;
    const int foundSlot = FindPickupSlotNear(position, PICKUP_MATCH_RADIUS);
    if (foundSlot >= 0 && CPickups::aPickUps[foundSlot].m_nPickupType == PICKUP_MONEY)
    {
        spawnPos = CPickups::aPickUps[foundSlot].GetPosn();
    }

    const uint16_t clampedAmount = static_cast<uint16_t>(amount > 50000 ? 50000 : amount);
    SendPickupSpawn(spawnPos, 0, PICKUP_MONEY, clampedAmount, false);
}

void CPickupSync::NotifyJetpackPickedUp(const CVector& position)
{
    if (!CNetwork::m_bAuthenticated || s_bApplyingRemote)
    {
        return;
    }

    const int foundSlot = FindPickupSlotNear(position, PICKUP_MATCH_RADIUS, MODEL_JETPACK);
    if (foundSlot >= 0)
    {
        const CVector pickupPos = CPickups::aPickUps[foundSlot].GetPosn();
        SendPickupCollected(static_cast<uint16_t>(foundSlot), pickupPos, MODEL_JETPACK);
        s_hasLastJetpackDropPos = false;
        return;
    }

    if (s_hasLastJetpackDropPos)
    {
        NotifyCollectedAt(s_lastJetpackDropPos, MODEL_JETPACK);
        s_hasLastJetpackDropPos = false;
    }
}

void CPickupSync::ApplySpawned(const Packets::World::PickupSpawn& packet)
{
    s_bApplyingRemote = true;

    if (packet.pickupType == PICKUP_MONEY)
    {
        CPickups::CreateSomeMoney(packet.pos, static_cast<int>(packet.ammo));
    }
    else if (packet.modelId != 0)
    {
        CPickups::GenerateNewOne(
            packet.pos, packet.modelId, packet.pickupType, packet.ammo, 0, packet.isEmpty, nullptr);
    }

    s_bApplyingRemote = false;
}

void CPickupSync::HandleUpdateAll(const Packets::World::UpdateAllPickups& packet)
{
    for (uint16_t i = 0; i < packet.count; ++i)
    {
        const auto& entry = packet.entries[i];
        Packets::World::PickupCollected collected{};
        collected.pickupSlot = entry.pickupSlot;
        collected.pos_x = entry.pos_x;
        collected.pos_y = entry.pos_y;
        collected.pos_z = entry.pos_z;
        collected.modelId = entry.modelId;
        ApplyCollected(collected, false);
    }
}

bool CPickupSync::IsApplyingRemote()
{
    return s_bApplyingRemote;
}
