#include "stdafx.h"
#include "TaskHooks.h"
#include "CNetworkVehicle.h"
#include "CNetworkPed.h"
#include "CAimSync.h"
#include <CKeySync.h>

// when local player enters any vehicle
static void __fastcall CTaskComplexEnterCarAsDriver__Ctor_Hook(
    CTaskComplexEnterCarAsDriver* This, SKIP_EDX, CVehicle* vehicle)
{
    CNetworkVehicle* networkVehicle = CNetworkVehicleManager::GetVehicle(vehicle);

    if (networkVehicle == nullptr)
    {
        //CChat::AddMessage("ERROR: this vehicle is not synced");
        plugin::CallMethod<0x6402F0, CTaskComplexEnterCarAsDriver*, CVehicle*>(This, vehicle);
        return;
    }

    Packets::Vehicles::VehicleEnter packet{};
    packet.seatid = 0;
    packet.vehicleid = networkVehicle->m_nVehicleId;
    GetPacketFactory().Send(packet);

    plugin::CallMethod<0x6402F0, CTaskComplexEnterCarAsDriver*, CVehicle*>(This, vehicle);
}

static void __fastcall CTaskComplexLeaveCar__Ctor_Hook(CTaskComplexLeaveCar* This, SKIP_EDX, CVehicle* vehicle,
    int targetDoor, int delayTime, bool sensibleLeaveCar, bool forceGetOut)
{
    if (CNetwork::m_bAuthenticated)
    {
        Packets::Vehicles::VehicleExit packet{};
        GetPacketFactory().Send(packet);
    }

    plugin::CallMethod<0x63B8C0, CTaskComplexLeaveCar*, CVehicle*, int, int, bool, bool>(
        This, vehicle, targetDoor, delayTime, sensibleLeaveCar, forceGetOut);
}


bool __fastcall CTaskSimpleUseGun__SetPedPosition_Hook(CTaskSimpleUseGun* This, SKIP_EDX, CPed* pPed)
{
    if (!pPed->IsPlayer() || pPed == FindPlayerPed(0))
    {
        return plugin::CallMethodAndReturn<bool, 0x624ED0>(This, pPed);
    }

    CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(pPed);
    if (pNetworkPlayer == nullptr)
    {
        return plugin::CallMethodAndReturn<bool, 0x624ED0>(This, pPed);
    }

    int playerNum = pNetworkPlayer->GetInternalId();
    if (playerNum == -1)
    {
        return plugin::CallMethodAndReturn<bool, 0x624ED0>(This, pPed);
    }

    CWorld::PlayerInFocus = playerNum;
    CKeySync::ApplyNetworkPlayerContext(pNetworkPlayer);
    CAimSync::ApplyNetworkPlayerContext(pNetworkPlayer);

    bool result = plugin::CallMethodAndReturn<bool, 0x624ED0>(This, pPed);

    CWorld::PlayerInFocus = 0;
    CKeySync::ApplyLocalContext();
    CAimSync::ApplyLocalContext();

    return result;
}

void TaskHooks::InjectHooks()
{
    patch::RedirectCall(0x570A1B, CTaskComplexEnterCarAsDriver__Ctor_Hook);
    patch::RedirectCall(0x570A94, CTaskComplexEnterCarAsDriver__Ctor_Hook);

    patch::RedirectCall(0x57049C, CTaskComplexLeaveCar__Ctor_Hook);
    patch::RedirectCall(0x5703F7, CTaskComplexLeaveCar__Ctor_Hook);

    // patch::RedirectCall(0x62D59B, CTaskSimpleGangDriveBy__SetupStaticAnimForPlayer_Hook);

    // patch::RedirectCall(0x62D813, CTaskSimpleGangDriveBy__ProcessAiming_Hook);

    patch::SetPointer(0x86D744, CTaskSimpleUseGun__SetPedPosition_Hook);
}
