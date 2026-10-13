#include "CPassengerEnter.h"
#include "stdafx.h"
#include "CNetworkVehicle.h"

#include <CCarEnterExit.h>
#include <CTaskComplexEnterCarAsPassenger.h>

static bool bNearPassegnerDoorMessageDisplayed = false;

// TODO(v0.3.1-alpha): use internal game's functions to find the nearest vehicle, look here 0x57073E

bool IsPlayerEnteringVehicle(CPlayerPed* pPlayerPed)
{
    static const int taskTypes[] = {701, 700, 713, 712, 718, 800};
    for (int taskType : taskTypes)
    {
        if (pPlayerPed->m_pIntelligence->m_TaskMgr.FindActiveTaskByType(taskType))
            return true;
    }

    return false;
}

bool HasFoundNearbyVehiclePassengerDoor()
{
    float fNearestVehDistance = 99999999.0f;
    CVehicle* pNearestVeh = nullptr;

    CPlayerPed* pPlayerPed = FindPlayerPed(0);
    CVector vecPlayerPosition = pPlayerPed->GetPosition();

    for (auto* pVehicle : CPools::ms_pVehiclePool)
    {
        if (pVehicle == nullptr)
        {
            continue;
        }

        if (pVehicle->m_nMaxPassengers == 0)
        {
            continue;
        }

        float fDistance = (vecPlayerPosition - pVehicle->GetPosition()).Magnitude();

        if(fDistance > pVehicle->GetColModel()->m_boundSphere.m_fRadius)
        {
            continue;
        }

        if (fDistance < fNearestVehDistance)
        {
            fNearestVehDistance = fDistance;
            pNearestVeh = pVehicle;
        }
    }

    if (pNearestVeh == nullptr)
    {
        return false;
    }

    int doorId = 0;
    CVector temp;

    return CCarEnterExit::GetNearestCarPassengerDoor(pPlayerPed, pNearestVeh, &temp, &doorId, true, true, true);
}

void UpdatePassengerDoorHint()
{
    if (bNearPassegnerDoorMessageDisplayed)
    {
        return;
    }

    if (HasFoundNearbyVehiclePassengerDoor())
    {
        CHud::SetHelpMessage("Press G to enter the vehicle as a passenger.", false, false, false);

        bNearPassegnerDoorMessageDisplayed = true;
    }
}

void CPassengerEnter::Process()
{
    CPlayerPed* pPlayerPed = FindPlayerPed(0);

    if (pPlayerPed == nullptr)
        return;

    if (pPlayerPed->m_nPedFlags.bInVehicle)
        return;

    if (CTimer::m_snTimeInMilliseconds / 1000 != CTimer::m_snPreviousTimeInMilliseconds / 1000)
    {
        UpdatePassengerDoorHint();
    }
        
    if (IsPlayerEnteringVehicle(pPlayerPed))
        return;

    CPad* pPad = pPlayerPed->GetPadFromPlayer();

    if (!pPad->OldState.DPadUp && pPad->NewState.DPadUp && !pPad->bDisablePlayerEnterCar &&
        pPad->DisablePlayerControls == 0)  // G key
    {
        CNetworkVehicle* pMinNetworkVehicle = nullptr;
        float fMinDistance = 99999999.0f;

        for (auto pNetworkVehicle : CNetworkVehicleManager::m_pVehicles)
        {
            if (auto pVehicle = pNetworkVehicle->m_pVehicle)
            {
                float fLength = (pVehicle->m_matrix->pos - pPlayerPed->m_matrix->pos).Magnitude();

                if (fLength < fMinDistance)
                {
                    pMinNetworkVehicle = pNetworkVehicle;
                    fMinDistance = fLength;
                }
            }
        }

        if (fMinDistance <= 8.0f)
        {
            int doorId = 0;
            CVector temp;  // not checked by nullptr in the game, so we should use temporary var
            CVehicle* pMinVehicle = pMinNetworkVehicle->m_pVehicle;
            if (CCarEnterExit::GetNearestCarPassengerDoor(pPlayerPed, pMinVehicle, &temp, &doorId, true, true, true))
            {
                CTaskComplexEnterCarAsPassenger* pEnterCarTask = new CTaskComplexEnterCarAsPassenger(pMinVehicle, doorId, false);
                pPlayerPed->m_pIntelligence->m_TaskMgr.SetTask(pEnterCarTask, 3, false);

                Packets::Vehicles::VehicleEnter packet{};
                packet.vehicleid = pMinNetworkVehicle->m_vehicleId;
                packet.seatid = CCarEnterExit::ComputePassengerIndexFromCarDoor(pMinVehicle, doorId);
                packet.bForce = false;
                packet.bPassenger = true;
                GetPacketFactory().Send(packet);
            }
        }
    }
}