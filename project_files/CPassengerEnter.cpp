#include "stdafx.h"
#include "CNetworkVehicle.h"

#include <CCarEnterExit.h>
#include <CTaskComplexEnterCarAsPassenger.h>
#include <game_sa/CTaskComplexEnterCarAsPassengerTimed.h>

// LEGACY: use internal game's functions to find the nearest vehicle, look here 0x57073E

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

void CPassengerEnter::Process()
{
    CPlayerPed* localPlayer = FindPlayerPed(0);

    if (localPlayer == nullptr)
        return;

    if (localPlayer->m_nPedFlags.bInVehicle)
        return;

    if (IsPlayerEnteringVehicle(localPlayer))
        return;

    CPad* pad = localPlayer->GetPadFromPlayer();

    if (!pad->OldState.DPadUp && pad->NewState.DPadUp)  // G key
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

                CPackets::VehicleEnter packet{};

                packet.vehicleid = minNetworkVehicle->m_nVehicleId;
                packet.seatid = CCarEnterExit::ComputePassengerIndexFromCarDoor(minVehicle, doorId);
                packet.force = false;
                packet.passenger = true;

                CNetwork::SendPacket(CPacketsID::VEHICLE_ENTER, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
            }
        }
    }
}
