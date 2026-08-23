#include "CPassengerEnter.h"
#include "stdafx.h"
#include "CNetworkVehicle.h"

#include <CCarEnterExit.h>
#include <CTaskComplexEnterCarAsPassenger.h>

static bool bNearPassegnerDoorMessageDisplayed = false;

// TODO(v0.3.1-alpha): use internal game's functions to find the nearest vehicle, look here 0x57073E

bool IsPlayerEnteringVehicle(CPlayerPed* player)
{
    static const int taskTypes[] = {701, 700, 713, 712, 718, 800};
    for (int taskType : taskTypes)
    {
        if (player->m_pIntelligence->m_TaskMgr.FindActiveTaskByType(taskType))
            return true;
    }

    return false;
}

bool HasFoundNearbyVehiclePassengerDoor()
{
    float nearestVehDistance = 99999999.0f;
    CVehicle* nearestVeh = nullptr;

    CPlayerPed* pPlayerPed = FindPlayerPed(0);
    CVector playerPosition = pPlayerPed->GetPosition();

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

        float fDistance = (playerPosition - pVehicle->GetPosition()).Magnitude();

        if(fDistance > pVehicle->GetColModel()->m_boundSphere.m_fRadius)
        {
            continue;
        }

        if (fDistance < nearestVehDistance)
        {
            nearestVehDistance = fDistance;
            nearestVeh = pVehicle;
        }
    }

    if (nearestVeh == nullptr)
    {
        return false;
    }

    int doorId = 0;
    CVector temp;

    return CCarEnterExit::GetNearestCarPassengerDoor(pPlayerPed, nearestVeh, &temp, &doorId, true, true, true);
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
    CPlayerPed* localPlayer = FindPlayerPed(0);

    if (localPlayer == nullptr)
        return;

    if (localPlayer->m_nPedFlags.bInVehicle)
        return;

    if (CTimer::m_snTimeInMilliseconds / 1000 != CTimer::m_snPreviousTimeInMilliseconds / 1000)
    {
        UpdatePassengerDoorHint();
    }
        
    if (IsPlayerEnteringVehicle(localPlayer))
        return;

    CPad* pad = localPlayer->GetPadFromPlayer();

    if (!pad->OldState.DPadUp && pad->NewState.DPadUp && !pad->bDisablePlayerEnterCar &&
        pad->DisablePlayerControls == 0)  // G key
    {
        CNetworkVehicle* minNetworkVehicle = nullptr;
        float minDistance = 99999999.0f;

        for (auto networkVehicle : CNetworkVehicleManager::m_pVehicles)
        {
            if (auto vehicle = networkVehicle->m_pVehicle)
            {
                float length = (vehicle->m_matrix->pos - localPlayer->m_matrix->pos).Magnitude();

                if (length < minDistance)
                {
                    minNetworkVehicle = networkVehicle;
                    minDistance = length;
                }
            }
        }

        if (minDistance <= 8.0f)
        {
            int doorId = 0;
            CVector temp;  // not checked by nullptr in the game, so we should use temporary var
            CVehicle* minVehicle = minNetworkVehicle->m_pVehicle;
            if (CCarEnterExit::GetNearestCarPassengerDoor(localPlayer, minVehicle, &temp, &doorId, true, true, true))
            {
                CTaskComplexEnterCarAsPassenger* task = new CTaskComplexEnterCarAsPassenger(minVehicle, doorId, false);
                localPlayer->m_pIntelligence->m_TaskMgr.SetTask(task, 3, false);

                Packets::Vehicles::VehicleEnter packet{};
                packet.vehicleid = minNetworkVehicle->m_nVehicleId;
                packet.seatid = CCarEnterExit::ComputePassengerIndexFromCarDoor(minVehicle, doorId);
                packet.bForce = false;
                packet.bPassenger = true;
                GetPacketFactory().Send(packet);
            }
        }
    }
}