#include "stdafx.h"
#include "TaskHooks.h"
#include "CTaskSync.h"
#include "CNetworkVehicle.h"
#include "CNetworkPed.h"
#include "CAimSync.h"

// when local player enters any vehicle
static void __fastcall CTaskComplexEnterCarAsDriver__Ctor_Hook(CTaskComplexEnterCarAsDriver* This, int, CVehicle* vehicle)
{
    CNetworkVehicle* networkVehicle = CNetworkVehicleManager::GetVehicle(vehicle);

    if (networkVehicle == nullptr)
    {
        CChat::AddMessage("ERROR: this vehicle is not synced");
        plugin::CallMethod<0x6402F0, CTaskComplexEnterCarAsDriver*, CVehicle*>(This, vehicle);
        return;
    }

    CPackets::VehicleEnter packet{};

    packet.seatid = 0;
    packet.vehicleid = networkVehicle->m_nVehicleId;

    CNetwork::SendPacket(CPacketsID::VEHICLE_ENTER, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);

    plugin::CallMethod<0x6402F0, CTaskComplexEnterCarAsDriver*, CVehicle*>(This, vehicle);
}

static void __fastcall CTaskComplexLeaveCar__Ctor_Hook(CTaskComplexLeaveCar* This, int, CVehicle* vehicle, int targetDoor, int delayTime, bool sensibleLeaveCar, bool forceGetOut)
{
    CPackets::VehicleExit packet{};
    CNetwork::SendPacket(CPacketsID::VEHICLE_EXIT, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    plugin::CallMethod<0x63B8C0, CTaskComplexLeaveCar*, CVehicle*, int, int, bool, bool>(This, vehicle, targetDoor, delayTime, sensibleLeaveCar, forceGetOut);
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

    if (pTaskMgr && pTask)
    {
        pNetworkPed = CNetworkPedManager::GetPed(pTaskMgr->m_pPed);

        if (pNetworkPed && pNetworkPed->m_bSyncing)
        {
#ifdef TASK_LOG
            CChat::AddMessage("SET PRIMARY %s", CDebugPedTasks::TaskNames[pTask->GetId()]);
#endif
            nTaskPacketSize = 0;
            pSerializedTask = CTaskSync::SerializeTask(pTask, pNetworkPed, true, nTaskSlot, &nTaskPacketSize);

            if(nTaskPacketSize > 0)
                CNetwork::SendPacket(CPacketsID::PED_ADD_TASK, pSerializedTask, nTaskPacketSize, ENET_PACKET_FLAG_RELIABLE);
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
        pNetworkPed = CNetworkPedManager::GetPed(pTaskMgr->m_pPed);

        if (pNetworkPed)
        {
#ifdef TASK_LOG
            CChat::AddMessage("SET SECONDARY %s", CDebugPedTasks::TaskNames[pTask->GetId()]);
#endif
            nTaskPacketSize = 0;
            pSerializedTask = CTaskSync::SerializeTask(pTask, pNetworkPed, false, nTaskSlot, &nTaskPacketSize);

            if (nTaskPacketSize > 0)
                CNetwork::SendPacket(CPacketsID::PED_ADD_TASK, pSerializedTask, nTaskPacketSize, ENET_PACKET_FLAG_RELIABLE);
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

    if (pTask)
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
    
    if (pTaskSimple)
    {
        vtable = *(void***)pTaskSimple; // get vtable 

        dwGetIdAddr = patch::GetUInt((DWORD)vtable + 16); // get `GetId`

        if (dwGetIdAddr == 0x82263A) // if virtual function is not implemented (purecall)
        {
            goto skip;
        }

        pNetworkPed = CUtil::GetNetworkPedByTask(pTaskSimple);

        if (pNetworkPed && pNetworkPed->m_bSyncing)
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

    if (pTaskComplex)
    {
        vtable = *(void***)pTaskComplex; // get vtable 

        dwGetIdAddr = patch::GetUInt((DWORD)vtable + 16); // get `GetId`

        if (dwGetIdAddr == 0x82263A) // if virtual function is not implemented (purecall)
        {
            goto skip;
        }

        pNetworkPed = CUtil::GetNetworkPedByTask(pTaskComplex);

        if (pNetworkPed && pNetworkPed->m_bSyncing)
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

bool __fastcall CTaskSimpleUseGun__SetPedPosition_Hook(CTaskSimpleUseGun* This, int, CPed* ped)
{
    auto orig = [&]() -> bool {
        return plugin::CallMethodAndReturn<bool, 0x624ED0>(This, ped);
    };

    if (ped->m_nPedType > 3 || ped == FindPlayerPed(0)) // not a network player
    {
        return orig();
    }

    if (auto networkPlayer = CNetworkPlayerManager::GetPlayer(ped))
    {
        int playerNum = networkPlayer->GetInternalId();

        if (playerNum == -1)
            return orig();

        CWorld::PlayerInFocus = playerNum;

        CStatsSync::ApplyNetworkPlayerContext(networkPlayer);
        CAimSync::ApplyNetworkPlayerContext(networkPlayer);
        bool result = orig();
        CAimSync::ApplyLocalContext();
        CStatsSync::ApplyLocalContext();
        CWorld::PlayerInFocus = 0;
        return result;
    }

    return orig();
}

void TaskHooks::InjectHooks()
{
    patch::RedirectCall(0x570A1B, CTaskComplexEnterCarAsDriver__Ctor_Hook);
    patch::RedirectCall(0x570A94, CTaskComplexEnterCarAsDriver__Ctor_Hook);

    patch::RedirectCall(0x57049C, CTaskComplexLeaveCar__Ctor_Hook);
    patch::RedirectCall(0x5703F7, CTaskComplexLeaveCar__Ctor_Hook);

    //patch::RedirectCall(0x62D59B, CTaskSimpleGangDriveBy__SetupStaticAnimForPlayer_Hook);

    //patch::RedirectCall(0x62D813, CTaskSimpleGangDriveBy__ProcessAiming_Hook);

    // ped tasks hooks (help me im going crazy)
    patch::RedirectJump(0x681AF0, CTaskManager__SetTask_Hook);
    patch::RedirectJump(0x681B60, CTaskManager__SetTaskSecondary_Hook);
    patch::RedirectJump(0x61A449, CTaskComplex__SetSubTask_Hook);
    patch::RedirectJump(0x61A3A0, CTaskSimple__dtor_Hook);
    patch::RedirectJump(0x61A413, CTaskComplex__dtor_Hook);

    patch::SetPointer(0x86D744, CTaskSimpleUseGun__SetPedPosition_Hook);
}
