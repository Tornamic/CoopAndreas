#include "CDriveBy.h"
#include "stdafx.h"

void CDriveBy::Process(CPlayerPed* player)
{
    bool isPassenger = player->m_nPedFlags.bInVehicle && player->m_pVehicle && player->m_pVehicle->m_pDriver != player;

    if (isPassenger && player->GetPadFromPlayer()->HornJustDown())
    {
        CDriveBy::StartDriveby(player);
    }
}

bool CDriveBy::IsPedInDriveby(CPed* ped)
{
    return ped->m_pIntelligence->m_TaskMgr.FindActiveTaskByType(eTaskType::TASK_SIMPLE_GANG_DRIVEBY) != nullptr;
}

void CDriveBy::StartDriveby(CPed* ped)
{
    plugin::Command<0x713>(CPools::GetPedRef(ped), -1, -1, 0.0f, 0.0f, 0.0f, 300.0f, 8, 1, 100);
}

void CDriveBy::StopDriveby(CPed* ped)
{
    CTask* task = ped->m_pIntelligence->m_TaskMgr.FindActiveTaskByType(eTaskType::TASK_SIMPLE_GANG_DRIVEBY);

    if (CDriveBy::IsPedInDriveby(ped))
    {
        ped->m_pIntelligence->ClearTasks(true, true);
    }
}