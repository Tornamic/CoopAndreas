#include "stdafx.h"
#include "CTaskSync.h"
#include <CTaskComplexJump.h>
#include <CTaskComplexWander.h>
#include <CTaskComplexClimb.h>
#include <CTaskSimpleCarSetPedInAsDriver.h>
#include "Tasks/CTaskComplexCarDriveWander.h"

#define PUSH(val) \
    memcpy(currentPtr, &val, sizeof(val)); \
    currentPtr += sizeof(val);

#define TASK_SIZE(size) \
    data = malloc(size + 4 + 4 + 1 + 1); \
    currentPtr = (char*)data; \
    memset(data, 0, size + 4 + 4 + 1 + 1); \
    PUSH(pedId) \
    PUSH(taskId) \
    PUSH(taskSlot) \
    PUSH(bPrimary)

#define READ(val) \
    memcpy(&val, currentPtr, sizeof(val)); \
    currentPtr += sizeof(val);

#define APPLY_TASK() \
    if(task) \
    {   \
        if (bPrimary) \
            ped->m_pIntelligence->m_TaskMgr.SetTask(task, taskSlot, false); \
        else \
            ped->m_pIntelligence->m_TaskMgr.SetTaskSecondary(task, taskSlot); \
    }

unsigned char GetTaskSlot(CTask* t, CPed* owner, bool bPrimary)
{
    if (bPrimary)
    {
        for (unsigned char i = 0; i < 5; i++)
        {
            CTask* task = owner->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[i];
            while (task)
            {
                if (task == t)
                    return i;

                task = task->GetSubTask();
            }
        }
    }
    else
    {
        for (unsigned char i = 0; i < 5; i++)
        {
            CTask* task = owner->m_pIntelligence->m_TaskMgr.m_aSecondaryTasks[i];
            while (task)
            {
                if (task == t)
                    return i;

                task = task->GetSubTask();
            }
        }
    }

    return 0;
}

bool IsPurecall(void** vtable, unsigned int idx)
{
    return patch::GetUInt((unsigned int)vtable+idx, false) == PURECALL;
}

void* CTaskSync::SerializeTask(CTask* t, CNetworkPed* owner, bool bPrimary, unsigned char taskSlot, size_t* dataSize)
{
    void* data = nullptr;
    char* currentPtr = nullptr;

    int pedId = owner->m_nPedId;
    int taskId = t->GetId();

    bool b = false; // temp bool var for casting 1b bool values

    void** vtable = *(void***)t;

    // okaaaayy lessgoooo
    switch (taskId)
    {
    case TASK_COMPLEX_CLIMB: // yeah, some types of tasks have same params
    case TASK_COMPLEX_JUMP:
    {
        CTaskComplexJump* task = (CTaskComplexJump*)t;
        TASK_SIZE(4);
        PUSH(task->m_nType); // eComplexJumpType
        break;
    }
    case TASK_COMPLEX_WANDER:
    {
        CTaskComplexWander* task = (CTaskComplexWander*)t;
        TASK_SIZE(14);

        PUSH(task->m_nMoveState);
        PUSH(task->m_nDir);
        b = task->m_bWanderSensibly;
        PUSH(b);
        PUSH(task->m_fTargetRadius);
        break;
    }
    /*case TASK_COMPLEX_CAR_DRIVE_WANDER:
    {
        CTaskComplexCarDriveWander* task = (CTaskComplexCarDriveWander*)t;

        CNetworkVehicle* networkVehicle = CNetworkVehicleManager::GetVehicle(owner->m_pPed->m_pVehicle);

        if (networkVehicle == nullptr)
        {
            break;
        }

        TASK_SIZE(9);

        PUSH(networkVehicle->m_nVehicleId);
        PUSH(task->m_CarDrivingStyle);
        PUSH(task->m_CruiseSpeed);
        break;
    }*/
    }

    if (dataSize) {
        *dataSize = currentPtr - (char*)data;
    }

    return data;
}

void CTaskSync::DeSerializeTask(void* data)
{
    char* currentPtr = (char*)data;

    int pedId = 0;
    int taskId = 0;
    unsigned char taskSlot = 0;
    bool bPrimary = false;


    bool b = false;

    READ(pedId);
    READ(taskId);
    READ(taskSlot);
    READ(bPrimary);

    CChat::AddMessage("DeSerializeTask %d %d %s %d %s", pedId, taskId, CDebugPedTasks::TaskNames[taskId], taskSlot, bPrimary ? "True" : "False");

    CNetworkPed* networkPed = CNetworkPedManager::GetPed(pedId);

    if (networkPed == nullptr || networkPed->m_pPed == nullptr)
        return;

    CPed* ped = networkPed->m_pPed;

    switch (taskId)
    {
    case TASK_COMPLEX_CLIMB:
    case TASK_COMPLEX_JUMP:
    {
        unsigned int m_nType;

        READ(m_nType);

        CTaskComplexJump* task = new CTaskComplexJump(m_nType);
        if(taskId == TASK_COMPLEX_CLIMB)
            task = new CTaskComplexClimb();

        APPLY_TASK();
        break;
    }
    case TASK_COMPLEX_WANDER:
    {
        CTaskComplexWander* task = plugin::CallAndReturn<CTaskComplexWander*, 0x673D00>(networkPed->m_pPed);

        READ(task->m_nMoveState);
        READ(task->m_nDir);
        READ(b);
        task->m_bWanderSensibly = b;
        READ(task->m_fTargetRadius);
        
        APPLY_TASK();
        break;
    }
    case TASK_COMPLEX_CAR_DRIVE_WANDER:
    {
        int vehicleId = 0;
        uint8_t carDrivingStyle = 0;
        float cruiseSpeed = 0.0f;

        READ(vehicleId);
        READ(carDrivingStyle);
        READ(cruiseSpeed);

        CNetworkVehicle* networkVehicle = CNetworkVehicleManager::GetVehicle(vehicleId);

        if (networkVehicle == nullptr || networkVehicle->m_pVehicle == nullptr)
            return;

        CTaskComplexCarDriveWander* task = new CTaskComplexCarDriveWander(networkVehicle->m_pVehicle, carDrivingStyle, cruiseSpeed);

        APPLY_TASK();
        break;
    }
    }
}