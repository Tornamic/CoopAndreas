#include "../stdafx.h"
#include "VehicleHooks.h"
#include "../CKeySync.h"
#include "../Entity/Types/CNetworkVehicle.h"
#include "../Entity/Types/CNetworkPed.h"
#include "../Entity/Manager/Types/CNetworkPlayerManager.h"
#include "../Entity/Manager/Types/CNetworkPedManager.h"
#include "../Entity/Manager/Types/CNetworkVehicleManager.h"

#include "../CLocalPlayer.h"
#include "../CPackets.h"
#include "../CNetwork.h"

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

    CNetworkPlayer* player = CNetworkPlayerManager::Instance().Get(vehicle->m_pDriver);

    if (player == nullptr)
    {
        if (!CLocalPlayer::m_bIsHost)
        {
            CNetworkPed* ped = CNetworkPedManager::Instance().Get(vehicle->m_pDriver);
            if (ped == nullptr)
            {
                plugin::CallMethod<0x502280, CAEVehicleAudioEntity*>(&vehicle->m_vehicleAudio);
                plugin::CallMethodDyn<CVehicle*>(call_addr, vehicle);
                return;
            }
            vehicle->m_autoPilot = ped->m_autoPilot;
            vehicle->m_fGasPedal = ped->m_fGasPedal;
            vehicle->m_fBreakPedal = ped->m_fBreakPedal;
            vehicle->m_fSteerAngle = ped->m_fSteerAngle;
        }
        plugin::CallMethod<0x502280, CAEVehicleAudioEntity*>(&vehicle->m_vehicleAudio);
        plugin::CallMethodDyn<CVehicle*>(call_addr, vehicle);
        return;
    }

    CWorld::PlayerInFocus = player->GetInternalId();

    CKeySync::ApplyNetworkPlayerContext(player);

    auto& syncData = player->GetSyncData();

    player->m_pEntity->m_fHealth = syncData.m_nHealth;
    player->m_pEntity->m_fArmour = syncData.m_nArmour;

    player->m_pEntity->m_nPedType = PED_TYPE_CIVMALE;

    plugin::CallMethod<0x502280, CAEVehicleAudioEntity*>(&vehicle->m_vehicleAudio);

    player->m_pEntity->m_nPedType = PED_TYPE_PLAYER1;


    /*bool savedLookingLeft = *(bool*)0xB6F1A4;
    bool savedLookingRight = *(bool*)0xB6F1A5;
    *(bool*)0xB6F1A4 = player->m_lOnFoot->controllerState.LeftShoulder2 > 0;
    *(bool*)0xB6F1A5 = player->m_lOnFoot->controllerState.RightShoulder2 > 0;*/

    plugin::CallMethodDyn<CVehicle*>(call_addr, vehicle);

    CWorld::PlayerInFocus = 0;

    CKeySync::ApplyLocalContext();

    /**(bool*)0xB6F1A4 = savedLookingLeft;
    *(bool*)0xB6F1A5 = savedLookingRight;*/
}

static void __fastcall CCarCtrl__RemoveDistantCars_Hook()
{
    if (CLocalPlayer::m_bIsHost)
        CCarCtrl::RemoveDistantCars();
}

static void __fastcall CVehicle__AddVehicleUpgrade_Hook(CVehicle* This, int, int modelid)
{
    if (auto vehicle = CNetworkVehicleManager::Instance().Get(This))
    {
        CPackets::VehicleComponentAdd packet{};
        packet.m_nVehicleId = vehicle->GetId();
        packet.m_nComponentId = modelid;
        CNetwork::SendPacket(ePacketType::VEHICLE_COMPONENT_ADD, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    }
    This->AddVehicleUpgrade(modelid);
}

static void __fastcall CVehicle__RemoveVehicleUpgrade_Hook(CVehicle* This, int, int modelid)
{
    if (auto vehicle = CNetworkVehicleManager::Instance().Get(This))
    {
        CPackets::VehicleComponentRemove packet{};
        packet.m_nVehicleId = vehicle->GetId();
        packet.m_nComponentId = modelid;
        CNetwork::SendPacket(ePacketType::VEHICLE_COMPONENT_REMOVE, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    }
    This->RemoveVehicleUpgrade(modelid);
}
CAutomobile* _automobile = nullptr;
CNetworkVehicle* _vehicle = nullptr;
static void __declspec(naked) CAutomobile__FireTruckControl_Hook()
{
    _asm mov _automobile, ecx
    _asm mov eax, [ecx]

        if (_automobile->m_pDriver == FindPlayerPed(0))
        {
            _automobile->FireTruckControl(0.0f);
        }
        else if (_vehicle = CNetworkVehicleManager::Instance().Get(_automobile))
        {
            _automobile->m_fDoomHorizontalRotation = _vehicle->m_fAimHorizontal;
            _automobile->m_fDoomVerticalRotation = _vehicle->m_fAimVertical;
            _automobile->FireTruckControl(0.0f);
            _automobile->m_fDoomHorizontalRotation = _vehicle->m_fAimHorizontal;
            _automobile->m_fDoomVerticalRotation = _vehicle->m_fAimVertical;
        }

    _asm retn 4
}

static void __fastcall CAutomobile__TankControl_Hook()
{
    CAutomobile* automobile = nullptr;
    _asm mov automobile, ecx
    _asm mov eax, [ecx]

        if (automobile->m_pDriver == FindPlayerPed(0))
        {
            automobile->TankControl();
            return;
        }

    CNetworkVehicle* vehicle = CNetworkVehicleManager::Instance().Get(automobile);
    if (vehicle != nullptr && automobile->m_pDriver != nullptr)
    {
        automobile->m_fDoomHorizontalRotation = vehicle->m_fAimHorizontal;
        automobile->m_fDoomVerticalRotation = vehicle->m_fAimVertical;
        automobile->TankControl();
        automobile->m_fDoomHorizontalRotation = vehicle->m_fAimHorizontal;
        automobile->m_fDoomVerticalRotation = vehicle->m_fAimVertical;
    }
}

static bool __fastcall CDamageManager__ApplyDamage_Hook(CDamageManager* This, int, CAutomobile* dm_comp, tComponent compId, float intensity, float a5)
{
    CNetworkVehicle* vehicle = CNetworkVehicleManager::Instance().Get(dm_comp);
    if (vehicle != nullptr && CLocalPlayer::m_bIsHost)
    {
        CPackets::VehicleDamage packet{};
        packet.m_nVehicleId = vehicle->GetId();
        packet.m_damageManager = dm_comp->m_damageManager;
        CNetwork::SendPacket(ePacketType::VEHICLE_DAMAGE, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    }
    return plugin::CallMethodAndReturn<bool, 0x6C24B0, CDamageManager*, CAutomobile*, tComponent, float, float>(This, dm_comp, compId, intensity, a5);
}

static void __fastcall CVehicle__SetRemap_Hook(CVehicle* This, int, int paintJobId)
{
    if (auto vehicle = CNetworkVehicleManager::Instance().Get(This))
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

    if (!CLocalPlayer::m_bIsHost)
    {
        CNetworkPed* networkPed = CNetworkPedManager::Instance().Get(This->m_pDriver);

        if (networkPed == nullptr)
        {
            return plugin::CallMethodAndReturnDyn<bool, CAutomobile*>(call_addr, This, a1);
        }
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

    patch::RedirectCall(0x6B1F5E, CAutomobile__FireTruckControl_Hook);
    patch::RedirectCall(0x6B2028, CAutomobile__TankControl_Hook);

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
}