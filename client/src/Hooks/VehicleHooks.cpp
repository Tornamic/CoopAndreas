#include "stdafx.h"
#include "VehicleHooks.h"
#include "CKeySync.h"
#include "CAimSync.h"
#include "CNetworkVehicle.h"
#include "CNetworkPed.h"
#include <CCarGenerator.h>
#include <CPopCycle.h>

void __fastcall CVehicle__ProcessControl_Hook()
{
    CVehicle* vehicle = nullptr;
    DWORD vtbl = 0;
    DWORD call_addr = 0;

    _asm mov vehicle, ecx
    _asm mov eax, [ecx]
        _asm mov vtbl, eax

    if (vtbl == 0x871120)      // CAutomobile
        call_addr = 0x6B1880;
    else if (vtbl == 0x8721A0) // CBoat
        call_addr = 0x6F1770;
    else if (vtbl == 0x871360) // CBike
        call_addr = 0x6B9250;
    else if (vtbl == 0x871948) // CPlane
        call_addr = 0x6C9260;
    else if (vtbl == 0x871680) // CHeli
        call_addr = 0x6C7050;
    else if (vtbl == 0x871528) // CBmx
        call_addr = 0x6BFA30;
    else if (vtbl == 0x8717d8) // CMonsterTruck
        call_addr = 0x6C8250;
    else if (vtbl == 0x871AE8) // CQuadBike
        call_addr = 0x6CDCC0;
    else if (vtbl == 0x872370) // CTrain
        call_addr = 0x6F86A0;

    if (vehicle->m_pDriver == FindPlayerPed(0))
    {
        plugin::CallMethod<0x502280, CAEVehicleAudioEntity*>(&vehicle->m_vehicleAudio);
        plugin::CallMethodDyn<CVehicle*>(call_addr, vehicle);
        return;
    }

    CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(vehicle->m_pDriver);

    if (player == nullptr)
    {
        plugin::CallMethod<0x502280, CAEVehicleAudioEntity*>(&vehicle->m_vehicleAudio);
        plugin::CallMethodDyn<CVehicle*>(call_addr, vehicle);

        //CNetworkPed* ped = CNetworkPedManager::GetPed(vehicle->m_pDriver);
        //if (ped && !ped->m_bSyncing)
        //{
        //    //memset(&vehicle->m_autoPilot, 0, sizeof CAutoPilot);
        //    vehicle->m_fGasPedal = ped->m_fGasPedal;
        //    vehicle->m_fBreakPedal = ped->m_fBreakPedal;
        //    vehicle->m_fSteerAngle = ped->m_fSteerAngle;
        //}
        return;
    }

    int playerNum = player->GetInternalId();

    if (playerNum == -1)
    {
        plugin::CallMethod<0x502280, CAEVehicleAudioEntity*>(&vehicle->m_vehicleAudio);
        plugin::CallMethodDyn<CVehicle*>(call_addr, vehicle);
        return;
    }

    CWorld::PlayerInFocus = playerNum;

    CKeySync::ApplyNetworkPlayerContext(player);
    CAimSync::ApplyNetworkPlayerContext(player);
    //CStatsSync::ApplyNetworkPlayerContext(player);

    player->m_pPed->m_fHealth = player->m_lOnFoot->health;
    player->m_pPed->m_fArmour = player->m_lOnFoot->armour;

    player->m_pPed->m_nPedType = PED_TYPE_CIVMALE;

    plugin::CallMethod<0x502280, CAEVehicleAudioEntity*>(&vehicle->m_vehicleAudio);

    player->m_pPed->m_nPedType = PED_TYPE_PLAYER1;


    bool savedLookingLeft = *(bool*)0xB6F1A4;
    bool savedLookingRight = *(bool*)0xB6F1A5;
    *(bool*)0xB6F1A4 = player->m_newControllerState.LeftShoulder2 > 0;
    *(bool*)0xB6F1A5 = player->m_newControllerState.RightShoulder2 > 0;

    plugin::CallMethodDyn<CVehicle*>(call_addr, vehicle);

    CWorld::PlayerInFocus = 0;

    CKeySync::ApplyLocalContext();
    CAimSync::ApplyLocalContext();
   // CStatsSync::ApplyLocalContext();

    *(bool*)0xB6F1A4 = savedLookingLeft;
    *(bool*)0xB6F1A5 = savedLookingRight;
}

static void __fastcall CCarCtrl__RemoveDistantCars_Hook()
{
    //if (CLocalPlayer::m_bIsHost)
        CCarCtrl::RemoveDistantCars();
}

static void __fastcall CVehicle__AddVehicleUpgrade_Hook(CVehicle* This, int, int modelid)
{
    if (auto vehicle = CNetworkVehicleManager::GetVehicle(This))
    {
        CPackets::VehicleComponentAdd packet{};
        packet.vehicleid = vehicle->m_nVehicleId;
        packet.componentid = modelid;
        CNetwork::SendPacket(CPacketsID::VEHICLE_COMPONENT_ADD, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    }
    This->AddVehicleUpgrade(modelid);
}

static void __fastcall CVehicle__RemoveVehicleUpgrade_Hook(CVehicle* This, int, int modelid)
{
    if (auto vehicle = CNetworkVehicleManager::GetVehicle(This))
    {
        CPackets::VehicleComponentRemove packet{};
        packet.vehicleid = vehicle->m_nVehicleId;
        packet.componentid = modelid;
        CNetwork::SendPacket(CPacketsID::VEHICLE_COMPONENT_REMOVE, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    }
    This->RemoveVehicleUpgrade(modelid);
}

static bool __fastcall CDamageManager__ApplyDamage_Hook(CDamageManager* This, int, CAutomobile* dm_comp, tComponent compId, float intensity, float a5)
{
    CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(dm_comp);
    if (vehicle != nullptr && vehicle->m_bSyncing)
    {
        CPackets::VehicleDamage packet{};
        packet.vehicleid = vehicle->m_nVehicleId;
        packet.damageManager = dm_comp->m_damageManager;
        CNetwork::SendPacket(CPacketsID::VEHICLE_DAMAGE, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    }
    return plugin::CallMethodAndReturn<bool, 0x6C24B0, CDamageManager*, CAutomobile*, tComponent, float, float>(This, dm_comp, compId, intensity, a5);
}

static void __fastcall CVehicle__SetRemap_Hook(CVehicle* This, int, int paintJobId)
{
    if (auto vehicle = CNetworkVehicleManager::GetVehicle(This))
        vehicle->m_nPaintJob = paintJobId;

    This->SetRemap(paintJobId);
}

static bool __fastcall CAutomobile__ProcessAI_Hook(CAutomobile* This, int, int a1)
{
    DWORD vtbl = *(DWORD*)(This);
    DWORD call_addr = 0x6B4800;

    if (vtbl == 0x871360) // CBike
        call_addr = 0x6BC930;
    else if (vtbl == 0x871AE8) // CQuadBike
        call_addr = 0x6CE460;
    else if (vtbl == 0x871C28) // CTrailer
        call_addr = 0x6CF590;

    CNetworkPed* networkPed = CNetworkPedManager::GetPed(This->m_pDriver);
    if (networkPed && !networkPed->m_bSyncing)
    {
        This->m_autoPilot = networkPed->m_autoPilot;
        This->m_fGasPedal = networkPed->m_fGasPedal;
        This->m_fBreakPedal = networkPed->m_fBreakPedal;
        This->m_fSteerAngle = networkPed->m_fSteerAngle;

        bool result = plugin::CallMethodAndReturnDyn<bool, CAutomobile*>(call_addr, This, a1);

        This->m_autoPilot = networkPed->m_autoPilot;
        This->m_fGasPedal = networkPed->m_fGasPedal;
        This->m_fBreakPedal = networkPed->m_fBreakPedal;
        This->m_fSteerAngle = networkPed->m_fSteerAngle;

        return result;
    }

    return plugin::CallMethodAndReturnDyn<bool, CAutomobile*>(call_addr, This, a1);
}

// disallow creating a parked vehicle if it is created by another player (does not work perfectly)
// also disallow creating a parked vehicle if the player is dead, fixes vehicle pool overf*ck
bool __fastcall CCarGenerator__CheckForBlockage_Hook(CCarGenerator* This, int, int modelId)
{
    bool originalResult = This->CheckForBlockage(modelId);

    if (originalResult)
        return true;

    if (FindPlayerPed(0)->m_fHealth <= 0.0f) // if is dead
        return true;

    CVector position = This->m_vecPosn.Uncompressed();

    for (auto vehicle : CPools::ms_pVehiclePool)
    {
        if ((vehicle->GetPosition() - position).Magnitude() <= 3.0f)
        {
            return true;
        }
    }

    return false;
}

// the simplest method to allow other players 
// to create traffic vehicles without conflicts 
void CCarCtrl__GenerateOneRandomCar_Hook()
{
    int numCarsInRadius = 0;
    CVector pos = FindPlayerCoors(0);
    CPlayerPed* player = FindPlayerPed(0);

    for (auto vehicle : CPools::ms_pVehiclePool)
    {
        if ((vehicle->GetPosition() - pos).Magnitude() <= 100.0f)
        {
            numCarsInRadius++;
        }
    }

    int savedNumRandomCars =        patch::GetInt(0x969094, false);
    int savedNumLawEnforcerCars =   patch::GetInt(0x969098, false);
    int savedNumMissionCars =       patch::GetInt(0x96909C, false);
    int savedNumAmbulancesOnDuty =  patch::GetInt(0x9690A8, false);
    int savedNumFireTrucksOnDuty =  patch::GetInt(0x9690AC, false);
    patch::SetInt(0x969094, numCarsInRadius, false);
    patch::SetInt(0x969098, 0, false);
    patch::SetInt(0x96909C, 0, false);
    patch::SetInt(0x9690A8, 0, false);
    patch::SetInt(0x9690AC, 0, false);

    // also dont generate vehicles when the player is a passenger 
    // and his vehicle is driven by another player

    if (player->m_pVehicle && player->m_nPedFlags.bInVehicle) // if the local player in a vehicle
    {
        // generate cars only if the player is a ...
        if (player->m_pVehicle->m_pDriver == player // driver
            || player->m_pVehicle->m_pDriver == nullptr // passenger without driver 
            || !player->m_pVehicle->m_pDriver->IsPlayer()) // passenger with NPC driver
        {
            CCarCtrl::GenerateOneRandomCar(); // call original function
        }
    }
    else // on foot
    {
        CCarCtrl::GenerateOneRandomCar(); // call original function
    }

    patch::SetInt(0x969094, savedNumRandomCars, false);
    patch::SetInt(0x969098, savedNumLawEnforcerCars, false);
    patch::SetInt(0x96909C, savedNumMissionCars, false);
    patch::SetInt(0x9690A8, savedNumAmbulancesOnDuty, false);
    patch::SetInt(0x9690AC, savedNumFireTrucksOnDuty, false);
}

void CCarCtrl__UpdateCarAI_Hook(CVehicle* vehicle)
{
    if (auto networkVehicle = CNetworkVehicleManager::GetVehicle(vehicle))
    {
        if (!networkVehicle->m_bSyncing)
        {
            if (auto ped = vehicle->m_pDriver)
            {
                if (auto networkPed = CNetworkPedManager::GetPed(ped))
                {
                    if (!networkPed->m_bSyncing)
                    {
                        vehicle->m_fGasPedal = networkPed->m_fGasPedal;
                        vehicle->m_fBreakPedal = networkPed->m_fBreakPedal;
                        return;
                    }
                }
            }
        }
    }
    plugin::Call<0x41DA30>(vehicle);
}

void __fastcall CVehicle__SetVehicleCreatedBy_Hook(CVehicle* This, int, int createdBy)
{
    This->SetVehicleCreatedBy(createdBy);

    if (!CLocalPlayer::m_bIsHost)
        return;

    if (auto networkVehicle = CNetworkVehicleManager::GetVehicle(This))
    {
        CPackets::SetVehicleCreatedBy packet{};
        packet.vehicleid = networkVehicle->m_nVehicleId;
        packet.createdBy = static_cast<uint8_t>(createdBy);
        CNetwork::SendPacket(CPacketsID::SET_VEHICLE_CREATED_BY, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    }
}

void CTheScripts__CleanUpThisVehicle_Hook(CVehicle* vehicle)
{
    CTheScripts::CleanUpThisVehicle(vehicle);

    if (!CLocalPlayer::m_bIsHost)
        return;

    if (auto networkVehicle = CNetworkVehicleManager::GetVehicle(vehicle))
    {
        CPackets::SetVehicleCreatedBy packet{};
        packet.vehicleid = networkVehicle->m_nVehicleId;
        packet.createdBy = static_cast<uint8_t>(vehicle->m_nCreatedBy);
        CNetwork::SendPacket(CPacketsID::SET_VEHICLE_CREATED_BY, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    }
}

CPed* BmxFix_FindPlayerPed_Hook(CVehicle* vehicle)
{
    return vehicle->m_pDriver;
}

void __fastcall CPlayerPed__DoStuffToGoOnFire_Hook(CPlayerPed* This, int)
{
    if (This != nullptr && This->IsPlayer())
    {
        This->DoStuffToGoOnFire();
    }
}

void __fastcall CFireManager__StartFire_Hook(void* This, int, CPed* entity, CEntity* playerPed, float a4, int a5, int time, uint8_t numGenerations)
{
    if (entity != nullptr)
    {
        plugin::CallMethod<0x53A050>(This, entity, playerPed, a4, a5, time, numGenerations);
    }
}

void VehicleHooks::InjectHooks()
{
    patch::RedirectCall(0x53C1CB, CCarCtrl__RemoveDistantCars_Hook);

    patch::RedirectCall(0x46DCE4, CDamageManager__ApplyDamage_Hook);
    patch::RedirectCall(0x46DFFD, CDamageManager__ApplyDamage_Hook);
    patch::RedirectCall(0x6A7C2A, CDamageManager__ApplyDamage_Hook);
    patch::RedirectCall(0x6A7DD5, CDamageManager__ApplyDamage_Hook);
    patch::RedirectCall(0x6A7E07, CDamageManager__ApplyDamage_Hook);
    patch::RedirectCall(0x6A7E39, CDamageManager__ApplyDamage_Hook);
    patch::RedirectCall(0x6A7E91, CDamageManager__ApplyDamage_Hook);
    patch::RedirectCall(0x6A7F5B, CDamageManager__ApplyDamage_Hook);
    patch::RedirectCall(0x6A7F8D, CDamageManager__ApplyDamage_Hook);
    patch::RedirectCall(0x6A803F, CDamageManager__ApplyDamage_Hook);

    patch::RedirectCall(0x44828D, CVehicle__SetRemap_Hook);
    patch::RedirectCall(0x44B184, CVehicle__SetRemap_Hook);
    patch::RedirectCall(0x49872F, CVehicle__SetRemap_Hook);

    patch::RedirectCall(0x431998, CVehicle__AddVehicleUpgrade_Hook);
    patch::RedirectCall(0x4732DE, CVehicle__AddVehicleUpgrade_Hook);
    patch::RedirectCall(0x4985DA, CVehicle__AddVehicleUpgrade_Hook);
    patch::RedirectCall(0x49B106, CVehicle__AddVehicleUpgrade_Hook);
    patch::RedirectCall(0x6B0BD7, CVehicle__AddVehicleUpgrade_Hook);
    patch::RedirectCall(0x6E3424, CVehicle__AddVehicleUpgrade_Hook);

    patch::RedirectCall(0x4732F2, CVehicle__RemoveVehicleUpgrade_Hook);
    patch::RedirectCall(0x498618, CVehicle__RemoveVehicleUpgrade_Hook);

    patch::SetPointer(0x871148, CVehicle__ProcessControl_Hook);
    patch::SetPointer(0x8721C8, CVehicle__ProcessControl_Hook);
    patch::SetPointer(0x871388, CVehicle__ProcessControl_Hook);
    patch::SetPointer(0x871970, CVehicle__ProcessControl_Hook);
    patch::SetPointer(0x8716A8, CVehicle__ProcessControl_Hook);
    patch::SetPointer(0x871550, CVehicle__ProcessControl_Hook);
    patch::SetPointer(0x871800, CVehicle__ProcessControl_Hook);
    patch::SetPointer(0x871B10, CVehicle__ProcessControl_Hook);
    patch::SetPointer(0x872398, CVehicle__ProcessControl_Hook);

    //patch::SetPointer(0x871228, CAutomobile__ProcessAI_Hook); // CAutomobile  

    patch::RedirectCall(0x6F35D6, CCarGenerator__CheckForBlockage_Hook);
    patch::RedirectCall(0x6F35FF, CCarGenerator__CheckForBlockage_Hook);

    patch::RedirectCall(0x434263, CCarCtrl__GenerateOneRandomCar_Hook);
    patch::RedirectCall(0x434268, CCarCtrl__GenerateOneRandomCar_Hook);

    patch::RedirectCall(0x6B51CD, CCarCtrl__UpdateCarAI_Hook);
    patch::RedirectCall(0x6B52A2, CCarCtrl__UpdateCarAI_Hook);
    patch::RedirectCall(0x6BCC64, CCarCtrl__UpdateCarAI_Hook);
    patch::RedirectCall(0x6BCD7B, CCarCtrl__UpdateCarAI_Hook); 
    patch::RedirectCall(0x6C1B72, CCarCtrl__UpdateCarAI_Hook);
    patch::RedirectCall(0x6C1C5B, CCarCtrl__UpdateCarAI_Hook);
    patch::RedirectCall(0x6F1925, CCarCtrl__UpdateCarAI_Hook);
    patch::RedirectCall(0x6F1979, CCarCtrl__UpdateCarAI_Hook);

    patch::RedirectCall(0x445A20, CVehicle__SetVehicleCreatedBy_Hook);
    patch::RedirectCall(0x445E64, CVehicle__SetVehicleCreatedBy_Hook);
    patch::RedirectCall(0x469506, CVehicle__SetVehicleCreatedBy_Hook);
    patch::RedirectCall(0x469542, CVehicle__SetVehicleCreatedBy_Hook);
    patch::RedirectCall(0x469567, CVehicle__SetVehicleCreatedBy_Hook);
    patch::RedirectCall(0x46EC89, CVehicle__SetVehicleCreatedBy_Hook);
    patch::RedirectCall(0x4815E1, CVehicle__SetVehicleCreatedBy_Hook);
    patch::RedirectCall(0x6F3C33, CVehicle__SetVehicleCreatedBy_Hook);

    if (CCore::gvm.IsHoodlum())
    {
        patch::RedirectCall(0x156217F, CVehicle__SetVehicleCreatedBy_Hook);
        
        patch::RedirectCall(0x468AEB, CTheScripts__CleanUpThisVehicle_Hook);
        patch::RedirectCall(0x47D775, CTheScripts__CleanUpThisVehicle_Hook);
        patch::RedirectCall(0x64CED4, CTheScripts__CleanUpThisVehicle_Hook);
        patch::RedirectCall(0x64D186, CTheScripts__CleanUpThisVehicle_Hook);
    }
    else
    {
        patch::RedirectCall(0x45AE9F, CVehicle__SetVehicleCreatedBy_Hook);
        patch::RedirectCall(0x4866AF, CVehicle__SetVehicleCreatedBy_Hook);
    }


    // fix bmx on fire, now driver being set on fire instead of the local player, compatible with SilentPatch
    patch::SetRaw(0x53A984, (void*)"\x90\x57", 2); // nop, push edi
    patch::SetRaw(0x53A9A7, (void*)"\x90\x57", 2); // nop, push edi
    patch::RedirectCall(0x53A984 + 2, BmxFix_FindPlayerPed_Hook);
    patch::RedirectCall(0x53A9A7 + 2, BmxFix_FindPlayerPed_Hook);
    patch::RedirectCall(0x53A990, CPlayerPed__DoStuffToGoOnFire_Hook);
    patch::RedirectCall(0x53A9B7, CFireManager__StartFire_Hook);
}