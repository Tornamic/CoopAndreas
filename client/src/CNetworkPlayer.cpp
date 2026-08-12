#include "stdafx.h"
#include <CTaskSimpleCarSetPedOut.h>
#include <CCarEnterExit.h>
#include <CTaskSimpleCarSetPedInAsPassenger.h>
#include <CTaskComplexEnterCarAsPassenger.h>

CNetworkPlayer::~CNetworkPlayer()
{
    if (m_pPed == nullptr)
        return;

    this->DestroyPed();
}

CNetworkPlayer::CNetworkPlayer(int id, CVector position)
{
    m_iPlayerId = id;

    m_pPedClothesDesc.SetTextureAndModel("VEST", "VEST", 0);
    m_pPedClothesDesc.SetTextureAndModel("JEANSDENIM", "JEANS", 2);
    m_pPedClothesDesc.SetTextureAndModel("SNEAKERBINCBLK", "SNEAKER", 3);
    m_pPedClothesDesc.SetTextureAndModel("PLAYER_FACE", "HEAD", 1);

    CreatePed(id, position);
}

void CNetworkPlayer::CreatePed(int id, CVector position)
{
    unsigned int actorId = 0;
    int playerId = id + 2;

    plugin::Command<Commands::CREATE_PLAYER>(playerId, position.x, position.y, position.z, &actorId);
    plugin::Command<Commands::GET_PLAYER_CHAR>(playerId, &actorId);

    m_pPed = (CPlayerPed*)CPools::GetPed(actorId);

    m_pPed->SetOrientation(0.0f, 0.0f, 0.0f);

    // set player immunies, he doesn't care about the pain now
    Command<Commands::SET_CHAR_PROOFS>(actorId, 0, 1, 1, 0, 0);

    *m_pPed->m_pPlayerData->m_pPedClothesDesc = m_pPedClothesDesc;

    CClothes::RebuildPlayer(m_pPed, false);
}

void CNetworkPlayer::DestroyPed()
{
    if (m_pPed->m_pVehicle)
    {
        plugin::Command<Commands::WARP_CHAR_FROM_CAR_TO_COORD>(CPools::GetPedRef(m_pPed), 0.f, 0.f, 0.f);
    }

    uintptr_t pedPtr = (uintptr_t)m_pPed;
    if (m_pPed->IsVTableValid())
    {
        CWorld::Remove(m_pPed);

        // destroy the ped
        __asm
        {
			mov ecx, pedPtr
			mov ebx, [ecx]  // vtable addr
			push 1  // unused arg
			call[ebx]  // call destructor
        }
    }
}

void CNetworkPlayer::Respawn()
{
    if (m_pPed)
    {
        this->DestroyPed();
    }

    this->CreatePed(m_iPlayerId, m_onFootSnapshotInterpolated.vecPos);
}

int CNetworkPlayer::GetInternalId()  // most used for CWorld::PlayerInFocus
{
    byte playerNumber = 0;

    for (; playerNumber < Config::MAX_SERVER_PLAYERS + 2; playerNumber++)
    {
        if (m_pPed == CWorld::Players[playerNumber].m_pPed)
        {
            return playerNumber;
        }
    }

    return -1;
}

std::string CNetworkPlayer::GetName()
{
    if (m_Name[0] == '\0')
    {
        char buffer[32 + 1];
        sprintf(buffer, "player %d", m_iPlayerId);
        return buffer;
    }

    return m_Name;
}

char CNetworkPlayer::GetWeaponSkill(eWeaponType weaponType)
{
    if (weaponType < WEAPON_PISTOL || weaponType > WEAPON_TEC9)
        return 1;

    eStats weaponStatId = plugin::CallAndReturn<eStats, 0x743CD0>(weaponType);  // CWeaponInfo::GetSkillStatIndex
    int statSyncId = CStatsSync::GetSyncIdByInternal(weaponStatId);
    float weaponStat = m_stats[weaponStatId];

    if (CWeaponInfo::GetWeaponInfo(weaponType, 2)->m_nReqStatLevel <= weaponStat)
        return 2;

    return CWeaponInfo::GetWeaponInfo(weaponType, 1)->m_nReqStatLevel <= weaponStat;
}

void CNetworkPlayer::WarpIntoVehicleDriver(CVehicle* vehicle)
{
    assert(m_pPed != nullptr);

    if (!vehicle->IsVTableValid() || !m_pPed->IsVTableValid())
    {
        return;
    }

    if (m_pPed->m_nPedFlags.bInVehicle && m_pPed->m_pVehicle)
    {
        RemoveFromVehicle(m_pPed->m_pVehicle);
    }

    m_pPed->m_pIntelligence->FlushImmediately(false);

    m_pPed->m_nPedFlags.CantBeKnockedOffBike = 1;  // 1 - never

    auto task = CTaskSimpleCarSetPedInAsDriver(vehicle, nullptr);
    task.m_bWarpingInToCar = true;
    task.ProcessPed(m_pPed);
}

void CNetworkPlayer::WarpIntoVehiclePassenger(CVehicle* vehicle, int seatid)
{
    assert(m_pPed != nullptr);

    if (!vehicle->IsVTableValid() || !m_pPed->IsVTableValid())
    {
        return;
    }

    if (m_pPed->m_nPedFlags.bInVehicle && m_pPed->m_pVehicle)
    {
        RemoveFromVehicle(m_pPed->m_pVehicle);
    }

    m_pPed->m_pIntelligence->FlushImmediately(false);

    m_pPed->m_nPedFlags.CantBeKnockedOffBike = 1;  // 1 - never

    int doorId = CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(vehicle, seatid);
    auto task = CTaskSimpleCarSetPedInAsPassenger(vehicle, doorId, nullptr);
    task.m_bWarpingInToCar = true;
    task.ProcessPed(m_pPed);
}

void CNetworkPlayer::EnterVehiclePassenger(CVehicle* vehicle, int seatid)
{
    assert(m_pPed != nullptr);

    if (!vehicle->IsVTableValid() || !m_pPed->IsVTableValid())
    {
        return;
    }

    if (m_pPed->m_nPedFlags.bInVehicle && m_pPed->m_pVehicle)
    {
        RemoveFromVehicle(m_pPed->m_pVehicle);
    }

    m_pPed->m_pIntelligence->FlushImmediately(false);

    m_pPed->m_nPedFlags.CantBeKnockedOffBike = 1;  // 1 - never

    int doorId = CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(vehicle, seatid);
    auto task = new CTaskComplexEnterCarAsPassenger(vehicle, doorId, false);
    m_pPed->m_pIntelligence->m_TaskMgr.SetTask(task, 3, false);
}

void CNetworkPlayer::RemoveFromVehicle(CVehicle* vehicle)
{
    assert(m_pPed != nullptr);

    if (!vehicle->IsVTableValid() || !m_pPed->IsVTableValid())
    {
        return;
    }

    m_pPed->m_pIntelligence->m_TaskMgr.SetTask(nullptr, TASK_PRIMARY_PRIMARY, false);

    m_pPed->m_nPedFlags.CantBeKnockedOffBike = 2;  // 2 - normal

    auto task = CTaskSimpleCarSetPedOut(vehicle, 1, false);
    task.m_bWarpingOutOfCar = true;
    task.ProcessPed(m_pPed);
}

void CNetworkPlayer::HandleTask(Packets::Players::SetPlayerTask& packet)
{
    if (!m_pPed)
    {
        return;
    }

#ifdef PACKET_DEBUG_MESSAGES
    CChat::AddMessage("HandleTask %d toggle %d", packet.taskType, packet.toggle);
#endif

    m_pPed->SetPosn(packet.vecPos);
    m_pPed->m_fCurrentRotation = packet.currentRotation.m_angle;
    m_pPed->m_fAimingRotation = packet.aimingRotation.m_angle;

    CTask* activeTask = m_pPed->m_pIntelligence->m_TaskMgr.GetActiveTask();
    eTaskType activeTaskType = activeTask ? activeTask->GetTaskType() : TASK_NONE;
    switch ((eTaskType)packet.taskType)
    {
        case eTaskType::TASK_COMPLEX_JUMP:
        {
            m_pPed->ClearWeaponTarget();
            if (activeTask && activeTaskType == packet.taskType)  // if the jump task is active
            {
                return;
            }

            if (m_pPed->m_pIntelligence->GetTaskDuck(false))
            {
                CTaskSimpleDuckToggle(0).ProcessPed(m_pPed);
            }

            m_pPed->m_pIntelligence->m_TaskMgr.SetTask(new CTaskComplexJump(0), 3, false);

            break;
        }
        case eTaskType::TASK_SIMPLE_DUCK:
        {
            m_bRequestedDuckTask = true;
            //if (packet.toggle)
            //{
            //    m_pPed->m_pIntelligence->SetTaskDuckSecondary(0);

            //    CTaskSimpleFight* pFightingTask = m_pPed->m_pIntelligence->GetTaskFighting();
            //    if (pFightingTask)
            //    {
            //        // abort fighting task (0x6879C2)
            //        plugin::CallMethodAndReturn<bool, 0x61C5E0, CTaskSimpleFight*, CEntity*, int8_t>(pFightingTask, nullptr, 18);
            //    }
            //}
            //else
            //{
            //    m_pPed->m_pIntelligence->ClearTaskDuckSecondary();
            //}
            break;
        }
    }
}

void CNetworkPlayer::ApplyWeaponSnapshot(Packets::Players::SWeaponSnapshot& weaponSnapshot)
{
    if (m_pPed == nullptr)
    {
        return;
    }

    m_onFootSnapshotInterpolated.weaponSnapshot = weaponSnapshot;
    // TODO refactor CUtil
    CUtil::GiveWeaponByPacket(this, weaponSnapshot.iWeaponType, weaponSnapshot.nAmmo);
    m_pPed->m_aWeapons[m_pPed->m_nActiveWeaponSlot].m_nState = static_cast<eWeaponState>(weaponSnapshot.iWeaponState);
}
