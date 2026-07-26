#include "network/packets/peds.h"
#include "network/packet_types.h"
#include "stdafx.h"
#include <CCarEnterExit.h>
#include <CTaskSimpleCarSetPedInAsPassenger.h>

PACKET_HANDLER(ePacketType::PED_SPAWN, Packets::Peds::PedSpawn* pPedSpawn)
{
#ifdef PACKET_DEBUG_MESSAGES
    CChat::AddMessage("PED SPAWN %d %d %.1f %.1f %.1f %d %d", packet->pedid, packet->modelId, packet->pos.x,
        packet->pos.y, packet->pos.z, packet->pedType, packet->createdBy);
#endif

    CNetworkPed* pNetworkPed = new CNetworkPed(pPedSpawn->pedid, pPedSpawn->modelId, pPedSpawn->pedType, pPedSpawn->pos,
        pPedSpawn->createdBy, pPedSpawn->specialModelName);

    CNetworkPedManager::Add(pNetworkPed);
}

PACKET_HANDLER(ePacketType::PED_CONFIRM, Packets::Peds::PedConfirm* pPedConfirm)
{
#ifdef PACKET_DEBUG_MESSAGES
    CChat::AddMessage("PED CONFIRM %d %d", packet->pedid, packet->tempid);
#endif

    if (pPedConfirm->tempid < ARRAY_SIZE(CNetworkPedManager::m_apTempPeds))
    {
        CNetworkPed* pTempPed = CNetworkPedManager::m_apTempPeds[pPedConfirm->tempid];
        if (pTempPed)
        {
            pTempPed->m_nPedId = pPedConfirm->pedid;
            CNetworkPedManager::Add(pTempPed);
            CNetworkPedManager::m_apTempPeds[pPedConfirm->tempid] = nullptr;
        }
    }
}

PACKET_HANDLER(ePacketType::PED_REMOVE, Packets::Peds::PedRemove* pPedRemove)
{
#ifdef PACKET_DEBUG_MESSAGES
    CChat::AddMessage("PED REMOVE %d", pPedRemove->pedid);
#endif

    CNetworkPed* pNetworkPed = CNetworkPedManager::GetPed(pPedRemove->pedid);
    if (pNetworkPed)
    {
        CNetworkPedManager::Remove(pNetworkPed);
        delete pNetworkPed;
    }
}

PACKET_HANDLER(ePacketType::ASSIGN_PED, Packets::Peds::AssignPedSyncer* pAssignPedSyncer)
{
    CNetworkPed* pNetworkPed = CNetworkPedManager::GetPed(pAssignPedSyncer->pedid);

    if (!pNetworkPed)
    {
        return;
    }

    if (pNetworkPed->m_bSyncing)
    {
#ifdef PACKET_DEBUG_MESSAGES
        CChat::AddMessage("NOT SYNCING PED %d ANYMORE", pAssignPedSyncer->pedid);
#endif
        pNetworkPed->m_bSyncing = false;

        if (auto pPed = pNetworkPed->m_pPed)
        {
            pPed->SetCharCreatedBy(MISSION_CHAR);
        }
    }
    else
    {
#ifdef PACKET_DEBUG_MESSAGES
        CChat::AddMessage("SYNCING VEHICLE %d", pAssignPedSyncer->pedid);
#endif
        pNetworkPed->m_bSyncing = true;
        pNetworkPed->m_bClaimOnRelease = false;

        if (auto pPed = pNetworkPed->m_pPed)
        {
            pPed->SetCharCreatedBy(pNetworkPed->m_nCreatedBy);
        }
    }
}

PACKET_HANDLER(ePacketType::PED_ONFOOT, Packets::Peds::PedOnFoot* pPedOnFoot)
{
    CNetworkPed* pNetworkPed = CNetworkPedManager::GetPed(pPedOnFoot->pedid);

    if (!pNetworkPed)
    {
        return;
    }

    CPed* pPed = pNetworkPed->m_pPed;
    if (!pPed)
    {
        return;
    }

    CVehicle* pVehicle = pPed->m_pVehicle;
    if (pVehicle && pPed->m_nPedFlags.bInVehicle && pVehicle->IsVTableValid())
    {
        // plugin::Command<Commands::TASK_LEAVE_CAR>(CPools::GetPedRef(ped->m_pPed),
        // CPools::GetVehicleRef(ped->m_pPed->m_pVehicle));
        // plugin::Command<Commands::WARP_CHAR_FROM_CAR_TO_COORD>(CPools::GetPedRef(ped->m_pPed), packet->pos.x,
        // packet->pos.y, packet->pos.z);
        pNetworkPed->RemoveFromVehicle(pVehicle);
    }

    pNetworkPed->ApplyWeaponSnapshot(pPedOnFoot->weaponSnapshot);
    pPed->SetPosn(pPedOnFoot->pos);

    pNetworkPed->m_fCurrentRotation = pPed->m_fCurrentRotation = pPedOnFoot->currentRotation.m_angle;
    pNetworkPed->m_fAimingRotation = pPed->m_fAimingRotation = pPedOnFoot->aimingRotation.m_angle;
    pNetworkPed->m_fLookDirection = pPed->m_fLookDirection = pPedOnFoot->lookDirection.m_angle;

    pNetworkPed->m_fHealth = pPed->m_fHealth = pPedOnFoot->healthSnapshot.iHealth;
    pPed->m_fArmour = pPedOnFoot->healthSnapshot.iArmour;

    pNetworkPed->m_vecVelocity = pPedOnFoot->velocity;
    pNetworkPed->m_nMoveState = pPedOnFoot->moveState;

    if (CUtil::IsDucked(pPed) != pPedOnFoot->bDucked)
    {
        CTaskSimpleDuckToggle task = CTaskSimpleDuckToggle(pPedOnFoot->bDucked);
        task.ProcessPed(pPed);
    }

    // TODO reimplement ped aim sync
    if (pPedOnFoot->bAiming)
    {
        CTaskSimpleUseGun* useGun = pPed->m_pIntelligence->GetTaskUseGun();
        if (!useGun)
        {
            auto* taskUseGun = new CTaskSimpleUseGun(pPed->m_pTargetedObject, CVector(0.0f, 0.0f, 0.f), 1, 1, false);
            pPed->m_pIntelligence->m_TaskMgr.SetTaskSecondary(taskUseGun, TASK_SECONDARY_ATTACK);
        }

        useGun = pPed->m_pIntelligence->GetTaskUseGun();

        if (useGun)
        {
            useGun->m_vecTarget = pPedOnFoot->weaponAim;
        }
    }
    else
    {
        if (auto useGun = pPed->m_pIntelligence->GetTaskUseGun())
        {
            useGun->MakeAbortable(pPed, ABORT_PRIORITY_URGENT, nullptr);
        }
    }

    pPed->m_nFightingStyle = pPedOnFoot->fightingStyle;
}

PACKET_HANDLER(ePacketType::PED_DRIVER_UPDATE, Packets::Peds::PedDriverUpdate* pPedDriverUpdate)
{
    CNetworkVehicle* pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pPedDriverUpdate->vehicleid);
    if (pNetworkVehicle == nullptr)
    {
        return;
    }

    CVehicle* pVehicle = pNetworkVehicle->m_pVehicle;
    if (pVehicle == nullptr || !pVehicle->IsVTableValid())
    {
        return;
    }

    CNetworkPed* pNetworkPed = CNetworkPedManager::GetPed(pPedDriverUpdate->pedid);
    if (pNetworkPed == nullptr)
    {
        return;
    }

    CPed* pPed = pNetworkPed->m_pPed;
    if (pPed == nullptr || !pPed->IsVTableValid())
    {
        return;
    }

    if (pPed->m_pVehicle != pVehicle || !pPed->m_nPedFlags.bInVehicle)
    {
        pNetworkPed->WarpIntoVehicleDriver(pVehicle);
    }
    pVehicle->m_matrix->pos = pPedDriverUpdate->pos;
    pVehicle->m_matrix->right = pPedDriverUpdate->roll;
    pVehicle->m_matrix->up = pPedDriverUpdate->rot;
    pNetworkPed->m_vecVelocity = pPedDriverUpdate->velocity;
    pVehicle->m_vecMoveSpeed = pPedDriverUpdate->velocity;
    pVehicle->m_vecTurnSpeed = pPedDriverUpdate->turnSpeed;

    pNetworkPed->ApplyWeaponSnapshot(pPedDriverUpdate->pedWeapon);

    pNetworkPed->m_fHealth = pPed->m_fHealth = pPedDriverUpdate->pedHealth.iHealth;
    pPed->m_fArmour = pPedDriverUpdate->pedHealth.iArmour;

    pVehicle->m_nPrimaryColor = pPedDriverUpdate->color1;
    pVehicle->m_nSecondaryColor = pPedDriverUpdate->color2;

    pVehicle->m_fHealth = pPedDriverUpdate->health;

    if (pNetworkVehicle->m_nPaintJob != pPedDriverUpdate->paintjob)
    {
        pVehicle->SetRemap(pPedDriverUpdate->paintjob);
    }

    if (pVehicle->m_nVehicleType == VEHICLE_BIKE)
    {
        CBike* pBike = (CBike*)pVehicle;
        pBike->m_rideAnimData.m_fDesiredLeanAngle = pPedDriverUpdate->bikeLean;
    }
    if (pVehicle->m_nVehicleSubType == VEHICLE_PLANE)
    {
        CPlane* pPlane = (CPlane*)pVehicle;
        pPlane->m_fLandingGearStatus = pPedDriverUpdate->planeGearState;
    }
    if (pVehicle->m_nVehicleSubType == VEHICLE_BMX)
    {
        CBmx* pBmx = (CBmx*)pVehicle;
        pBmx->m_fControlPedaling = pPedDriverUpdate->controlPedaling;
    }
    if (pVehicle->m_nVehicleType == VEHICLE_AUTOMOBILE)
    {
        ((CAutomobile*)pVehicle)->m_wMiscComponentAngle = pPedDriverUpdate->miscComponentAngle;
    }
    pVehicle->m_eDoorLock = pPedDriverUpdate->locked;

    pVehicle->m_fGasPedal = pNetworkPed->m_fGasPedal = pPedDriverUpdate->gasPedal;
    pVehicle->m_fBreakPedal = pNetworkPed->m_fBreakPedal = pPedDriverUpdate->breakPedal;
    pVehicle->m_fSteerAngle = pNetworkPed->m_fSteerAngle = pPedDriverUpdate->steerAngle;
}

PACKET_HANDLER(ePacketType::PED_PASSENGER_UPDATE, Packets::Peds::PedPassengerSync* pPedPassengerSync)
{
    CNetworkVehicle* pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pPedPassengerSync->vehicleid);
    CNetworkPed* pNetworkPed = CNetworkPedManager::GetPed(pPedPassengerSync->pedid);

    if (pNetworkVehicle == nullptr || pNetworkPed == nullptr)
        return;

    if (pNetworkVehicle->m_pVehicle == nullptr)
        return;

    if (pNetworkPed->m_pPed == nullptr)
        return;

    if (!pNetworkVehicle->m_pVehicle->IsVTableValid() || !pNetworkPed->m_pPed->IsVTableValid())
        return;

    if (!pNetworkPed->m_pPed->m_nPedFlags.bInVehicle || pNetworkVehicle->m_pVehicle->m_pDriver == pNetworkPed->m_pPed)
    {
        pNetworkPed->WarpIntoVehiclePassenger(pNetworkVehicle->m_pVehicle, pPedPassengerSync->seatid);
    }

    pNetworkPed->ApplyWeaponSnapshot(pPedPassengerSync->weaponSnapshot);

    pNetworkPed->m_fHealth = pNetworkPed->m_pPed->m_fHealth = pPedPassengerSync->healthSnapshot.iHealth;
    pNetworkPed->m_pPed->m_fArmour = pPedPassengerSync->healthSnapshot.iArmour;
}

PACKET_HANDLER(ePacketType::PED_SHOT_SYNC, Packets::Peds::PedShotSync* pPedShotSync)
{
    CNetworkPed* pNetworkPed = CNetworkPedManager::GetPed(pPedShotSync->pedid);

    if (pNetworkPed && pNetworkPed->m_pPed)
    {
        if (pNetworkPed->m_pPed->GetWeapon().m_eWeaponType != pPedShotSync->weaponType)
        {
            pNetworkPed->m_pPed->SetCurrentWeapon(pPedShotSync->weaponType);
        }
        pNetworkPed->m_pPed->GetWeapon().Fire(
            pNetworkPed->m_pPed, &pPedShotSync->origin, &pPedShotSync->effect, nullptr, &pPedShotSync->target, nullptr);
    }
}

PACKET_HANDLER(ePacketType::PED_SAY, Packets::Peds::PedSay* pPedSay)
{
    // CChat::AddMessage("PedSay %d %d %d %d %d", pPedSay->phraseId, pPedSay->startTimeDelay, pPedSay->overrideSilence,
    // pPedSay->isForceAudible, pPedSay->isFrontEnd);

    CPed* pPed = (CPed*)pPedSay->entity.GetEntity();
    if (pPed)
    {
        // CAEPedSpeechAudioEntity::AddSayEvent
        plugin::CallMethodAndReturn<int16_t, 0x4E6550, CAEPedSpeechAudioEntity*, int, int16_t, uint32_t, float, bool,
            bool, bool>(&pPed->m_pedSpeech, AE_SPEECH_PED, pPedSay->phraseId, pPedSay->startTimeDelay, 1.0f,
            pPedSay->overrideSilence, pPedSay->isForceAudible, pPedSay->isFrontEnd);
    }
}


PACKET_HANDLER(ePacketType::PED_RESET_ALL_CLAIMS, Packets::Peds::PedResetAllClaims* pPedResetAllClaims)
{
    if (auto pNetworkPed = CNetworkPedManager::GetPed(pPedResetAllClaims->pedid))
    {
        if (!pNetworkPed->m_bSyncing)
        {
            pNetworkPed->m_bClaimOnRelease = false;
        }
    }
}
