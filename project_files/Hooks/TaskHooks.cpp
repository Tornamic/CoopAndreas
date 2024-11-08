#include "../stdafx.h"
#include "TaskHooks.h"
#include "../CTaskSync.h"
#include "../Entity/Types/CNetworkVehicle.h"
#include "../Entity/Types/CNetworkPed.h"
#include "../Entity/Manager/Types/CNetworkVehicleManager.h"
#include "../CPackets.h"
#include "../CPackets.h"
#include "../CChat.h"
#include "../CNetwork.h"
#include "../Entity/Manager/Types/CNetworkPlayerManager.h"
#include "../CLocalPlayer.h"
#include "../Entity/Manager/Types/CNetworkPedManager.h"
#include "../CUtil.h"

// when local player enters any vehicle
static void __fastcall CTaskComplexEnterCarAsDriver__Ctor_Hook(CTaskComplexEnterCarAsDriver* This, int, CVehicle* vehicle)
{
    CNetworkVehicle* networkVehicle = CNetworkVehicleManager::Instance().Get(vehicle);

    if (networkVehicle == nullptr)
    {
        CChat::AddMessage("ERROR: this vehicle is not synced");
        plugin::CallMethod<0x6402F0, CTaskComplexEnterCarAsDriver*, CVehicle*>(This, vehicle);
        return;
    }

    CPackets::VehicleEnter packet{};

    packet.m_nSeatId = 0;
    packet.m_nVehicleId = networkVehicle->GetId();

    CNetwork::SendPacket(ePacketType::VEHICLE_ENTER, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);

    plugin::CallMethod<0x6402F0, CTaskComplexEnterCarAsDriver*, CVehicle*>(This, vehicle);
}

static void __fastcall CTaskComplexLeaveCar__Ctor_Hook(CTaskComplexLeaveCar* This, int, CVehicle* vehicle, int targetDoor, int delayTime, bool sensibleLeaveCar, bool forceGetOut)
{
    CPackets::VehicleExit packet{};
    CNetwork::SendPacket(ePacketType::VEHICLE_EXIT, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    plugin::CallMethod<0x63B8C0, CTaskComplexLeaveCar*, CVehicle*, int, int, bool, bool>(This, vehicle, targetDoor, delayTime, sensibleLeaveCar, forceGetOut);
}

static void __fastcall CTaskSimpleGangDriveBy__SetupStaticAnimForPlayer_Hook(CTaskSimpleGangDriveBy* This, int, CPed* ped)
{
    if (ped == FindPlayerPed(0) || !ped->IsPlayer())
    {
        plugin::CallMethod<0x621960, CTaskSimpleGangDriveBy*, CPed*>(This, ped);
        return;
    }

    CNetworkPlayer* player = CNetworkPlayerManager::Instance().Get(ped);

    CWorld::PlayerInFocus = player->GetInternalId();

    CVector savedAim = *(CVector*)0xB6F32C;
    eCamMode savedCameraMode = *(eCamMode*)0xB6F1A8;
    float savedAspectRatio = *(float*)0xC3EFA4;
    float savedExtZoom = *(float*)0xB6F250;

    *(CVector*)0xB6F32C = player->m_aPassengerAim;
    *(eCamMode*)0xB6F1A8 = eCamMode::MODE_AIMWEAPON_FROMCAR;
    *(float*)0xC3EFA4 = 0.0f;
    *(float*)0xB6F250 = 0.0f;

    plugin::CallMethod<0x621960, CTaskSimpleGangDriveBy*, CPed*>(This, ped);

    *(CVector*)0xB6F32C = savedAim;
    *(eCamMode*)0xB6F1A8 = savedCameraMode;
    *(float*)0xC3EFA4 = savedAspectRatio;
    *(float*)0xB6F250 = savedExtZoom;

    CWorld::PlayerInFocus = 0;
}

static void __fastcall CTaskSimpleGangDriveBy__ProcessAiming_Hook(CTaskSimpleGangDriveBy* This, int, CPed* ped)
{
    if (ped == FindPlayerPed(0) || !ped->IsPlayer())
    {
        plugin::CallMethod<0x628350, CTaskSimpleGangDriveBy*, CPed*>(This, ped);
        return;
    }

    ped->m_nPedType = ePedType::PED_TYPE_CIVMALE;
    plugin::CallMethod<0x628350, CTaskSimpleGangDriveBy*, CPed*>(This, ped);
    ped->m_nPedType = ePedType::PED_TYPE_PLAYER1;
}

CTaskManager* pTaskMgr = nullptr;
CTask* pTask = nullptr;
int nTasksId = 0;
CNetworkPed* pNetworkPed = nullptr;
DWORD CTaskManager__SetTask_Hook_Ret = 0x681AF5;
size_t nTaskPacketSize = 0;
void* pSerializedTask = nullptr;
int nTaskSlot = 0;
static void __declspec(naked) CTaskManager__SetTask_Hook()
{
    __asm
    {

        // original code
        push ebx
        mov ebx, [esp + 8]
        // -------------

        mov pTaskMgr, ecx // get *this

        pushad

        // get pTask
        mov eax, [esp + 10h]
        mov pTask, eax
        
        // get task slot
        mov eax, [esp + 14h]
        mov nTaskSlot, eax
    }

    if (CLocalPlayer::m_bIsHost &&  pTaskMgr && pTask)
    {
        pNetworkPed = CNetworkPedManager::Instance().Get(pTaskMgr->m_pPed);

        if (pNetworkPed)
        {
#ifdef TASK_LOG
            CChat::AddMessage("SET PRIMARY %s", CDebugPedTasks::TaskNames[pTask->GetId()]);
#endif
            nTaskPacketSize = 0;
            pSerializedTask = CTaskSync::SerializeTask(pTask, pNetworkPed, true, nTaskSlot, &nTaskPacketSize);

            if(nTaskPacketSize > 0)
                CNetwork::SendPacket(ePacketType::PED_ADD_TASK, pSerializedTask, nTaskPacketSize, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    __asm
    {
        popad

        jmp CTaskManager__SetTask_Hook_Ret
    }
}

DWORD CTaskManager__SetTaskSecondary_Hook_Ret = 0x681B65;
static void __declspec(naked) CTaskManager__SetTaskSecondary_Hook()
{
    __asm
    {

        // original code
        push ebx
        mov ebx, [esp + 8]
        // -------------

        mov pTaskMgr, ecx // get *this

        pushad

        // get pTask
        mov eax, [esp + 10h]
        mov pTask, eax

        // get task slot
        mov eax, [esp + 14h]
        mov nTaskSlot, eax
    }

    if (pTaskMgr && pTask)
    {
        pNetworkPed = CNetworkPedManager::Instance().Get(pTaskMgr->m_pPed);

        if (pNetworkPed)
        {
#ifdef TASK_LOG
            CChat::AddMessage("SET SECONDARY %s", CDebugPedTasks::TaskNames[pTask->GetId()]);
#endif
            nTaskPacketSize = 0;
            pSerializedTask = CTaskSync::SerializeTask(pTask, pNetworkPed, false, nTaskSlot, &nTaskPacketSize);

            if (nTaskPacketSize > 0)
                CNetwork::SendPacket(ePacketType::PED_ADD_TASK, pSerializedTask, nTaskPacketSize, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    __asm
    {
        popad

        jmp CTaskManager__SetTaskSecondary_Hook_Ret
    }
}


DWORD CTaskComplex__SetSubTask_Hook_Ret = 0x61A44E;
static void __declspec(naked) CTaskComplex__SetSubTask_Hook()
{
    __asm
    {
        // original code
        test edi, edi
        mov[esi + 8], edi
        // --------------

        pushad
        pushfd

        mov pTask, edi
    }

    if (CLocalPlayer::m_bIsHost && pTask)
    {
        pNetworkPed = CUtil::GetNetworkPedByTask(pTask);

#ifdef TASK_LOG
        if (pNetworkPed)
        {
            CChat::AddMessage("SET SUB %s", CDebugPedTasks::TaskNames[pTask->GetId()]);
        }
#endif
    }

    __asm
    {
        popfd
        popad

        jmp CTaskComplex__SetSubTask_Hook_Ret
    }
}

CTaskSimple* pTaskSimple = nullptr;
DWORD CTaskSimple__dtor_Hook_Ret = 0x61A3A6;
DWORD CTask_vtable = 0x86D48C;
void** vtable;
DWORD dwGetIdAddr = 0;
eTaskType nTaskType{};
static void __declspec(naked) CTaskSimple__dtor_Hook()
{
    __asm
    {
        pushad
        mov pTaskSimple, ecx
    }
    
    if (!CLocalPlayer::m_bIsHost)
        goto skip;

    if (pTaskSimple)
    {
        vtable = *(void***)pTaskSimple; // get vtable 

        dwGetIdAddr = patch::GetUInt((DWORD)vtable + 16); // get `GetId`

        if (dwGetIdAddr == 0x82263A) // if virtual function is not implemented (purecall)
        {
            goto skip;
        }

        pNetworkPed = CUtil::GetNetworkPedByTask(pTaskSimple);

        if (pNetworkPed)
        {
            nTaskType = plugin::CallMethodAndReturnDyn<eTaskType>(dwGetIdAddr, pTaskSimple);
#ifdef TASK_LOG
            CChat::AddMessage("DESTROY TASK %s", CDebugPedTasks::TaskNames[nTaskType]);
#endif // TASK_LOG

        }
    }

skip:
    __asm
    {
        popad
        mov dword ptr[ecx], offset CTask_vtable;
        jmp CTaskSimple__dtor_Hook_Ret
    }
}

CTaskComplex* pTaskComplex = nullptr;
DWORD CTaskComplex__dtor_Hook_Ret = 0x61A419;
static void __declspec(naked) CTaskComplex__dtor_Hook()
{
    __asm
    {
        pushad
        mov pTaskComplex, esi
    }

    if (!CLocalPlayer::m_bIsHost)
        goto skip;

    if (pTaskComplex)
    {
        vtable = *(void***)pTaskComplex; // get vtable 

        dwGetIdAddr = patch::GetUInt((DWORD)vtable + 16); // get `GetId`

        if (dwGetIdAddr == 0x82263A) // if virtual function is not implemented (purecall)
        {
            goto skip;
        }

        pNetworkPed = CUtil::GetNetworkPedByTask(pTaskComplex);

        if (pNetworkPed)
        {
            nTaskType = plugin::CallMethodAndReturnDyn<eTaskType>(dwGetIdAddr, pTaskComplex); 
#ifdef TASK_LOG
            CChat::AddMessage("DESTROY TASK %s", CDebugPedTasks::TaskNames[nTaskType]);
#endif
        }
    }

skip:
    __asm
    {
        popad
        mov dword ptr[esi], offset CTask_vtable;
        jmp CTaskComplex__dtor_Hook_Ret
    }
}

void TaskHooks::InjectHooks()
{
    patch::RedirectCall(0x570A1B, CTaskComplexEnterCarAsDriver__Ctor_Hook);
    patch::RedirectCall(0x570A94, CTaskComplexEnterCarAsDriver__Ctor_Hook);

    patch::RedirectCall(0x57049C, CTaskComplexLeaveCar__Ctor_Hook);
    patch::RedirectCall(0x5703F7, CTaskComplexLeaveCar__Ctor_Hook);

    patch::RedirectCall(0x62D59B, CTaskSimpleGangDriveBy__SetupStaticAnimForPlayer_Hook);

    patch::RedirectCall(0x62D813, CTaskSimpleGangDriveBy__ProcessAiming_Hook);

    // ped tasks hooks (help me im going crazy)
    patch::RedirectJump(0x681AF0, CTaskManager__SetTask_Hook);
    patch::RedirectJump(0x681B60, CTaskManager__SetTaskSecondary_Hook);
    patch::RedirectJump(0x61A449, CTaskComplex__SetSubTask_Hook);
    patch::RedirectJump(0x61A3A0, CTaskSimple__dtor_Hook);
    patch::RedirectJump(0x61A413, CTaskComplex__dtor_Hook);
}
