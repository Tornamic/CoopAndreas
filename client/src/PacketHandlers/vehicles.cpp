#include "network/packets/vehicles.h"
#include "network/packet_types.h"
#include "stdafx.h"
#include <CCarEnterExit.h>
#include <CTaskSimpleCarSetPedOut.h>

PACKET_HANDLER(ePacketType::VEHICLE_SPAWN, Packets::Vehicles::VehicleSpawn* pVehicleSpawn)
{
#ifdef PACKET_DEBUG_MESSAGES
    CChat::AddMessage("VEHICLE SPAWN %d %d %.1f %.1f %.1f %.1f", pVehicleSpawn->vehicleid, pVehicleSpawn->modelid,
        pVehicleSpawn->pos.x, pVehicleSpawn->pos.y, pVehicleSpawn->pos.z, pVehicleSpawn->rot.m_angle);
#endif

    CNetworkVehicle* pNetworkVehicle =
        new CNetworkVehicle(pVehicleSpawn->vehicleid, pVehicleSpawn->modelid, pVehicleSpawn->pos,
            pVehicleSpawn->rot.m_angle, pVehicleSpawn->color1, pVehicleSpawn->color2, pVehicleSpawn->createdBy);
    CNetworkVehicleManager::Add(pNetworkVehicle);
}

PACKET_HANDLER(ePacketType::VEHICLE_REMOVE, Packets::Vehicles::VehicleRemove* pVehicleRemove)
{
#ifdef PACKET_DEBUG_MESSAGES
    CChat::AddMessage("VEHICLE REMOVE %d", pVehicleRemove->vehicleid);
#endif
    CNetworkVehicle* pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pVehicleRemove->vehicleid);
    if (pNetworkVehicle)
    {
        CNetworkVehicleManager::Remove(pNetworkVehicle);
        delete pNetworkVehicle;
    }
}

PACKET_HANDLER(ePacketType::VEHICLE_CONFIRM, Packets::Vehicles::VehicleConfirm* pVehicleConfirm)
{
#ifdef PACKET_DEBUG_MESSAGES
    CChat::AddMessage("VEHICLE CONFIRM %d %d", pVehicleConfirm->vehicleid, pVehicleConfirm->tempid);
#endif

    if (pVehicleConfirm->tempid < ARRAY_SIZE(CNetworkVehicleManager::m_apTempVehicles))
    {
        CNetworkVehicle* pTempVehicle = CNetworkVehicleManager::m_apTempVehicles[pVehicleConfirm->tempid];
        if (pTempVehicle)
        {
            pTempVehicle->m_nVehicleId = pVehicleConfirm->vehicleid;
            CNetworkVehicleManager::Add(pTempVehicle);
            CNetworkVehicleManager::m_apTempVehicles[pVehicleConfirm->tempid] = nullptr;
        }
    }
}

PACKET_HANDLER(ePacketType::VEHICLE_IDLE_UPDATE, Packets::Vehicles::VehicleIdleUpdate* pVehicleIdleUpdate)
{
    CNetworkVehicle* pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pVehicleIdleUpdate->vehicleid);

    if (pNetworkVehicle == nullptr)
        return;

    if (!pNetworkVehicle->m_pVehicle)
        return;

    if (pNetworkVehicle->m_pVehicle->m_matrix == nullptr)
        return;

    pNetworkVehicle->m_pVehicle->m_matrix->pos = pVehicleIdleUpdate->pos;

    // if (CUtil::IsPositionUpdateNeeded(pNetworkVehicle->m_pVehicle->m_matrix->right, pVehicleIdleUpdate->roll, 1))
    pNetworkVehicle->m_pVehicle->m_matrix->right = pVehicleIdleUpdate->roll;

    // if (CUtil::IsPositionUpdateNeeded(pNetworkVehicle->m_pVehicle->m_matrix->up, pVehicleIdleUpdate->rot, 1))
    pNetworkVehicle->m_pVehicle->m_matrix->up = pVehicleIdleUpdate->rot;

    pNetworkVehicle->m_pVehicle->m_vecMoveSpeed = pVehicleIdleUpdate->velocity;
    pNetworkVehicle->m_pVehicle->m_vecTurnSpeed = pVehicleIdleUpdate->turnSpeed;

    pNetworkVehicle->m_pVehicle->m_nPrimaryColor = pVehicleIdleUpdate->color1;
    pNetworkVehicle->m_pVehicle->m_nSecondaryColor = pVehicleIdleUpdate->color2;
    pNetworkVehicle->m_pVehicle->m_fHealth = pVehicleIdleUpdate->health;

    if (pNetworkVehicle->m_pVehicle->GetRemapIndex() != pVehicleIdleUpdate->paintjob)
        pNetworkVehicle->m_pVehicle->SetRemap(pVehicleIdleUpdate->paintjob);

    if (CUtil::GetVehicleType(pNetworkVehicle->m_pVehicle) == eVehicleType::VEHICLE_PLANE)
    {
        CPlane* pPlane = (CPlane*)pNetworkVehicle->m_pVehicle;

        pPlane->m_fLandingGearStatus = pVehicleIdleUpdate->planeGearState;
    }

    pNetworkVehicle->m_pVehicle->m_eDoorLock = pVehicleIdleUpdate->locked;
}

PACKET_HANDLER(ePacketType::VEHICLE_DRIVER_UPDATE, Packets::Vehicles::VehicleDriverUpdate* pVehicleDriverUpdate)
{
    CNetworkVehicle* pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pVehicleDriverUpdate->vehicleid);
    CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(pVehicleDriverUpdate->playerid);

    if (pNetworkVehicle == nullptr || pNetworkPlayer == nullptr)
        return;

    CVehicle* pVehicle = pNetworkVehicle->m_pVehicle;
    if (!pVehicle->IsVTableValid())
        return;

    CVector driverPosition = pVehicleDriverUpdate->pos;
    CWantedSync::RecordVehicleDriverSource(pNetworkPlayer->m_pPed, driverPosition);

    if (pNetworkPlayer->m_pPed->m_pVehicle != pVehicle || !pNetworkPlayer->m_pPed->m_nPedFlags.bInVehicle)
    {
        pNetworkPlayer->WarpIntoVehicleDriver(pVehicle);
    }

    pVehicle->m_matrix->pos = pVehicleDriverUpdate->pos;
    pVehicle->m_matrix->right = pVehicleDriverUpdate->roll;
    pVehicle->m_matrix->up = pVehicleDriverUpdate->rot;
    pVehicle->m_vecMoveSpeed = pVehicleDriverUpdate->velocity;
    pVehicle->m_vecTurnSpeed = pVehicleDriverUpdate->turnSpeed;

    pNetworkPlayer->ApplyWeaponSnapshot(pVehicleDriverUpdate->playerWeapon);
    pNetworkPlayer->m_onFootSnapshotInterpolated.healthSnapshot = pVehicleDriverUpdate->playerHealth;
    pNetworkPlayer->m_pPed->m_fHealth = pVehicleDriverUpdate->playerHealth.iHealth;
    pNetworkPlayer->m_pPed->m_fArmour = pVehicleDriverUpdate->playerHealth.iArmour;
    pNetworkPlayer->m_oldControllerState = pVehicleDriverUpdate->playerKeys.oldControllerState;
    pNetworkPlayer->m_newControllerState = pVehicleDriverUpdate->playerKeys.newControllerState;

    pVehicle->m_nPrimaryColor = pVehicleDriverUpdate->color1;
    pVehicle->m_nSecondaryColor = pVehicleDriverUpdate->color2;

    pVehicle->m_fHealth = pVehicleDriverUpdate->health;

    if (pVehicle->GetRemapIndex() != pVehicleDriverUpdate->paintjob)
        pVehicle->SetRemap(pVehicleDriverUpdate->paintjob);

    if (pVehicle->m_nVehicleType == VEHICLE_AUTOMOBILE)
    {
        CAutomobile* pAutomobile = (CAutomobile*)pVehicle;
        pAutomobile->m_wMiscComponentAngle = pVehicleDriverUpdate->miscComponentAngle;
    }

    if (pVehicle->m_nVehicleType == VEHICLE_BIKE)
    {
        CBike* pBike = (CBike*)pVehicle;
        pBike->m_rideAnimData.m_fDesiredLeanAngle = pVehicleDriverUpdate->bikeLean;
    }

    if (pVehicle->m_nVehicleSubType == VEHICLE_PLANE)
    {
        CPlane* pPlane = (CPlane*)pVehicle;

        pPlane->m_fLandingGearStatus = pVehicleDriverUpdate->planeGearState;
    }

    pNetworkVehicle->m_pVehicle->m_eDoorLock = pVehicleDriverUpdate->locked;
    pNetworkVehicle->m_playerDriverSnapshot = *pVehicleDriverUpdate;
}

PACKET_HANDLER(ePacketType::VEHICLE_ENTER, Packets::Vehicles::VehicleEnter* pVehicleEnter)
{
    CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(pVehicleEnter->playerid);
    if (pNetworkPlayer == nullptr)
    {
        return;
    }

    CNetworkVehicle* pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pVehicleEnter->vehicleid);
    if (pNetworkVehicle == nullptr)
    {
        return;
    }

    CPlayerPed* pPlayerPed = pNetworkPlayer->m_pPed;
    CVehicle* pVehicle = pNetworkVehicle->m_pVehicle;

    if (!pPlayerPed->IsVTableValid() || !pVehicle->IsVTableValid())
        return;

    if (!pVehicleEnter->bPassenger)
    {
        CWantedSync::RecordVehicleEntrySource(pPlayerPed, pVehicle);
    }

#ifdef PACKET_DEBUG_MESSAGES
    CChat::AddMessage("player %d entered vehicleid %d %s", pVehicleEnter->playerid, pVehicleEnter->vehicleid,
        pVehicleEnter->seatid != 0 ? "as passenger" : "");
#endif

    // drop the jetpack if present
    if (auto pTask = pPlayerPed->m_pIntelligence->GetTaskJetPack())
    {
        pTask->MakeAbortable(pPlayerPed, ABORT_PRIORITY_URGENT, nullptr);
        pTask->m_nThrustStop = 0;
        pTask->m_nThrustFwd = 0;
        pTask->m_fThrustStrafe = 0.0f;
    }

    if (pVehicleEnter->bPassenger)
    {
        pNetworkPlayer->EnterVehiclePassenger(pVehicle, pVehicleEnter->seatid);
    }
    else
    {
        if (pVehicleEnter->bForce)
        {
            pNetworkPlayer->WarpIntoVehicleDriver(pVehicle);
        }
        else
        {
            pPlayerPed->m_nPedFlags.CantBeKnockedOffBike = 1;  // 1 - never
            auto pTask = new CTaskComplexEnterCarAsDriver(pVehicle);
            pPlayerPed->m_pIntelligence->m_TaskMgr.SetTask(pTask, TASK_PRIMARY_PRIMARY, false);
        }
    }
}

PACKET_HANDLER(ePacketType::VEHICLE_EXIT, Packets::Vehicles::VehicleExit* pVehicleExit)
{
    CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(pVehicleExit->playerid);

    if (pNetworkPlayer == nullptr)
    {
        return;
    }

    CPlayerPed* pPlayerPed = pNetworkPlayer->m_pPed;
    if (pPlayerPed->m_pVehicle == nullptr || !pPlayerPed->m_nPedFlags.bInVehicle)
    {
        return;
    }

    CVehicle* pVehicle = pPlayerPed->m_pVehicle;

    if (!pVehicle->IsVTableValid() || !pPlayerPed->IsVTableValid())
        return;

#ifdef PACKET_DEBUG_MESSAGES
    CChat::AddMessage("player %d exited from vehicle", pVehicleExit->playerid);
#endif

    if (pVehicleExit->bForce)
    {
        pPlayerPed->m_nPedFlags.CantBeKnockedOffBike = 2;  // 2 - normal
        CVector doorPos{};
        int door = CCarEnterExit::ComputeTargetDoorToExit(pVehicle, pPlayerPed);
        auto task = CTaskSimpleCarSetPedOut(pVehicle, door, false);
        task.ProcessPed(pPlayerPed);
    }
    else
    {
        pPlayerPed->m_nPedFlags.CantBeKnockedOffBike = 2;  // 2 - normal
        auto* task = new CTaskComplexLeaveCar(pVehicle, 0, 0, true, false);
        pPlayerPed->m_pIntelligence->m_TaskMgr.SetTask(task, TASK_PRIMARY_PRIMARY, false);
    }
}

PACKET_HANDLER(ePacketType::VEHICLE_DAMAGE, Packets::Vehicles::VehicleDamage* pVehicleDamage)
{
#ifdef PACKET_DEBUG_MESSAGES
    CChat::AddMessage("VehicleDamage__Handle");
#endif

    if (auto pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pVehicleDamage->vehicleid))
    {
        if (auto pVehicle = pNetworkVehicle->m_pVehicle)
        {
            if (pVehicle->IsVTableValid())
            {
                if (pVehicle->m_nVehicleType == VEHICLE_AUTOMOBILE)
                {
                    CAutomobile* pAutomobile = (CAutomobile*)pVehicle;
                    pAutomobile->m_damageManager = pVehicleDamage->damageManager;
                    pAutomobile->SetupDamageAfterLoad();
                    pNetworkVehicle->m_oldDamageState = pAutomobile->m_damageManager;
                }
            }
        }
    }
}

PACKET_HANDLER(ePacketType::VEHICLE_COMPONENT_ADD, Packets::Vehicles::VehicleComponentAdd* pVehicleComponentAdd)
{
    CNetworkVehicle* pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pVehicleComponentAdd->vehicleid);
    if (pNetworkVehicle == nullptr)
    {
        return;
    }

    CVehicle* pVehicle = pNetworkVehicle->m_pVehicle;
    if (!pVehicle || !pVehicle->IsVTableValid())
    {
        return;
    }

    CStreaming::RequestModel(pVehicleComponentAdd->componentid, eStreamingFlags::GAME_REQUIRED);
    CStreaming::LoadAllRequestedModels(false);
    CStreaming::RequestVehicleUpgrade(pVehicleComponentAdd->componentid, eStreamingFlags::GAME_REQUIRED);

    uint8_t count = 10;
    while (!CStreaming::HasVehicleUpgradeLoaded(pVehicleComponentAdd->componentid) && count)
    {
        Sleep(5);
        count--;
    }

    if (!count)
        return;

    pVehicle->AddVehicleUpgrade(pVehicleComponentAdd->componentid);
}

PACKET_HANDLER(
    ePacketType::VEHICLE_COMPONENT_REMOVE, Packets::Vehicles::VehicleComponentRemove* pVehicleComponentRemove)
{
    CNetworkVehicle* pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pVehicleComponentRemove->vehicleid);
    if (pNetworkVehicle == nullptr)
    {
        return;
    }

    CVehicle* pVehicle = pNetworkVehicle->m_pVehicle;
    if (!pVehicle || !pVehicle->IsVTableValid())
    {
        return;
    }

    pVehicle->RemoveVehicleUpgrade(pVehicleComponentRemove->componentid);
}

PACKET_HANDLER(
    ePacketType::VEHICLE_PASSENGER_UPDATE, Packets::Vehicles::VehiclePassengerUpdate* pVehiclePassengerUpdate)
{
    CNetworkVehicle* pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pVehiclePassengerUpdate->vehicleid);
    CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(pVehiclePassengerUpdate->playerid);

    if (pNetworkVehicle == nullptr || pNetworkPlayer == nullptr)
    {
        return;
    }

    CVehicle* pVehicle = pNetworkVehicle->m_pVehicle;
    CPlayerPed* pPlayerPed = pNetworkPlayer->m_pPed;

    if (pVehicle == nullptr || pPlayerPed == nullptr)
    {
        return;
    }

    if (!pVehicle->IsVTableValid() || !pPlayerPed->IsVTableValid())
    {
        return;
    }

    if (!pPlayerPed->m_nPedFlags.bInVehicle || pPlayerPed->m_pVehicle != pVehicle ||
        pNetworkVehicle->m_pVehicle->m_apPassengers[pVehiclePassengerUpdate->seatid] != pNetworkPlayer->m_pPed)
    {
#ifdef PACKET_DEBUG_MESSAGES
        CChat::AddMessage("forcing enter passenger %d", player->m_iPlayerId);
#endif

        pNetworkPlayer->WarpIntoVehiclePassenger(pNetworkVehicle->m_pVehicle, pVehiclePassengerUpdate->seatid);
    }

    pNetworkPlayer->ApplyWeaponSnapshot(pVehiclePassengerUpdate->playerWeapon);
    pNetworkPlayer->m_onFootSnapshotInterpolated.healthSnapshot = pVehiclePassengerUpdate->playerHealth;
    pNetworkPlayer->m_pPed->m_fHealth = pVehiclePassengerUpdate->playerHealth.iHealth;
    pNetworkPlayer->m_pPed->m_fArmour = pVehiclePassengerUpdate->playerHealth.iArmour;
    pNetworkPlayer->m_newControllerState = pVehiclePassengerUpdate->playerKeys.newControllerState;
    pNetworkPlayer->m_oldControllerState = pVehiclePassengerUpdate->playerKeys.oldControllerState;

    if (pVehiclePassengerUpdate->driveby && !CDriveBy::IsPedInDriveby(pPlayerPed))
    {
        CDriveBy::StartDriveby(pPlayerPed);
    }
    else if (!pVehiclePassengerUpdate->driveby && CDriveBy::IsPedInDriveby(pPlayerPed))
    {
        CDriveBy::StopDriveby(pPlayerPed);
    }
}

PACKET_HANDLER(ePacketType::ASSIGN_VEHICLE, Packets::Vehicles::AssignVehicleSyncer* pAssignVehicleSyncer)
{
    CNetworkVehicle* pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pAssignVehicleSyncer->vehicleid);

    if (!pNetworkVehicle)
    {
        return;
    }

    if (pNetworkVehicle->m_bSyncing)
    {
#ifdef PACKET_DEBUG_MESSAGES
        CChat::AddMessage("NOT SYNCING VEHICLE %d ANYMORE", pAssignVehicleSyncer->vehicleid);
#endif
        pNetworkVehicle->m_bSyncing = false;

        if (auto pVehicle = pNetworkVehicle->m_pVehicle)
        {
            pVehicle->SetVehicleCreatedBy(eVehicleCreatedBy::MISSION_VEHICLE);
        }
    }
    else
    {
#ifdef PACKET_DEBUG_MESSAGES
        CChat::AddMessage("SYNCING VEHICLE %d", pAssignVehicleSyncer->vehicleid);
#endif
        pNetworkVehicle->m_bSyncing = true;

        if (auto pVehicle = pNetworkVehicle->m_pVehicle)
        {
            pVehicle->SetVehicleCreatedBy(pNetworkVehicle->m_nCreatedBy);
        }
    }
}


PACKET_HANDLER(ePacketType::SET_VEHICLE_CREATED_BY, Packets::Vehicles::SetVehicleCreatedBy* pSetVehicleCreatedBy)
{
    if (CLocalPlayer::m_bIsHost)
    {
        return;
    }

    if (auto pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pSetVehicleCreatedBy->vehicleid))
    {
        pNetworkVehicle->m_nCreatedBy = pSetVehicleCreatedBy->createdBy;

        if (pNetworkVehicle->m_bSyncing)
        {
            if (auto pVehicle = pNetworkVehicle->m_pVehicle)
            {
                pVehicle->SetVehicleCreatedBy(pSetVehicleCreatedBy->createdBy);
            }
        }
    }
}