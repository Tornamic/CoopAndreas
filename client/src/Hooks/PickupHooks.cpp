#include "stdafx.h"
#include "PickupHooks.h"
#include "CPickupSync.h"
#include <CPickups.h>

namespace
{
    bool s_creatingMoney = false;

    void MaybeNotifyCollected(int pickupSlot, const CVector* pos = nullptr)
    {
        if (!CPickupSync::IsApplyingRemote())
        {
            CPickupSync::NotifyCollected(static_cast<uint16_t>(pickupSlot), pos);
        }
    }

    void CPickups__AddToCollectedPickupsArray_Hook(int pickupIndex)
    {
        const int uniqueHandle = CPickups::GetUniquePickupIndex(pickupIndex);
        CPickups::aPickUpsCollected[CPickups::CollectedPickUpIndex++] = uniqueHandle;
        CPickups::CollectedPickUpIndex %= static_cast<unsigned short>(MAX_COLLECTED_PICKUPS);

        MaybeNotifyCollected(pickupIndex);
    }

    void CPickups__RemovePickUp_Hook(int pickupHandle)
    {
        const int index = CPickups::GetActualPickupIndex(pickupHandle);
        if (index >= 0)
        {
            const CVector pos = CPickups::aPickUps[index].GetPosn();
            MaybeNotifyCollected(index, &pos);
            CPickups::aPickUps[index].Remove();
        }
    }

    bool __fastcall CPickup__Update_Hook(CPickup* pickup, SKIP_EDX, CPlayerPed* playerPed, CVehicle* vehicle, int playerId)
    {
        const CVector posBefore = pickup->GetPosn();
        const uint16_t modelId =
            pickup->m_nModelIndex >= 0 ? static_cast<uint16_t>(pickup->m_nModelIndex) : 0;

        const bool removed = pickup->Update(playerPed, vehicle, playerId);

        if (removed && !CPickupSync::IsApplyingRemote() && playerPed == FindPlayerPed(0) && modelId != 0)
        {
            CPickupSync::NotifyCollectedAt(posBefore, modelId);
        }

        return removed;
    }

    int __cdecl CPickups__GenerateNewOne_CallHook(
        CVector coors, unsigned int modelId, unsigned char pickupType, unsigned int ammo, unsigned int moneyPerDay,
        bool isEmpty, char* message)
    {
        const int result = CPickups::GenerateNewOne(coors, modelId, pickupType, ammo, moneyPerDay, isEmpty, message);

        if (!s_creatingMoney && !CPickupSync::IsApplyingRemote() && CNetwork::m_bAuthenticated
            && CPickupSync::ShouldSyncSpawnType(pickupType))
        {
            CPickupSync::NotifyPickupSpawned(
                coors, static_cast<uint16_t>(modelId), pickupType, static_cast<uint16_t>(ammo), isEmpty);
        }

        return result;
    }

    int __cdecl CPickups__GenerateNewOne_WeaponType_CallHook(
        CVector coors, eWeaponType weaponType, unsigned char pickupType, unsigned int ammo, bool isEmpty, char* message)
    {
        const int result = CPickups::GenerateNewOne_WeaponType(coors, weaponType, pickupType, ammo, isEmpty, message);

        if (!s_creatingMoney && !CPickupSync::IsApplyingRemote() && CNetwork::m_bAuthenticated
            && CPickupSync::ShouldSyncSpawnType(pickupType))
        {
            const uint16_t modelId = static_cast<uint16_t>(CPickups::ModelForWeapon(weaponType));
            CPickupSync::NotifyPickupSpawned(coors, modelId, pickupType, static_cast<uint16_t>(ammo), isEmpty);
        }

        return result;
    }

    void __cdecl CPickups__CreateSomeMoney_CallHook(CVector coors, int amount)
    {
        s_creatingMoney = true;
        CPickups::CreateSomeMoney(coors, amount);
        s_creatingMoney = false;

        if (!CPickupSync::IsApplyingRemote() && CNetwork::m_bAuthenticated)
        {
            CPickupSync::NotifyMoneySpawned(coors, amount);
        }
    }
}

void PickupHooks::InjectHooks()
{
    patch::RedirectJump(0x455240, CPickups__AddToCollectedPickupsArray_Hook);
    patch::RedirectJump(0x4573D0, CPickups__RemovePickUp_Hook);

    patch::RedirectCall(0x45902E, CPickup__Update_Hook);
    patch::RedirectCall(0x459095, CPickup__Update_Hook);

    const std::vector<int> generateNewOneCallSites = {
        0x00445098, 0x00445AFD, 0x00458A58, 0x00477983, 0x0047E636, 0x00480658, 0x00481744, 0x0048B243,
        0x0048CEB9, 0x00592103, 0x0067B6DE,
    };
    patch::RedirectCall(generateNewOneCallSites, CPickups__GenerateNewOne_CallHook);

    const std::vector<int> generateNewOneWeaponTypeCallSites = {0x004592F7, 0x005921B5};
    patch::RedirectCall(generateNewOneWeaponTypeCallSites, CPickups__GenerateNewOne_WeaponType_CallHook);

    const std::vector<int> createSomeMoneyCallSites = {0x00548FD6, 0x0054A97C};
    patch::RedirectCall(createSomeMoneyCallSites, CPickups__CreateSomeMoney_CallHook);
}
