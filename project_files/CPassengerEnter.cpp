#include "stdafx.h"
#include "CNetworkVehicle.h"
#include <Tasks/CTaskComplexEnterCarAsPassengerTimed.h>
#include <CCarEnterExit.h>
#include <CTaskComplexEnterCarAsPassenger.h>

void CPassengerEnter::Process()
{
	CPlayerPed* localPlayer = FindPlayerPed(0);

	if (localPlayer == nullptr)
		return;

	if (localPlayer->m_nPedFlags.bInVehicle)
		return;

	// is player entering any vehicle
	if (localPlayer->m_pIntelligence->m_TaskMgr.FindActiveTaskByType(701) ||
		localPlayer->m_pIntelligence->m_TaskMgr.FindActiveTaskByType(700) ||
		localPlayer->m_pIntelligence->m_TaskMgr.FindActiveTaskByType(713) ||
		localPlayer->m_pIntelligence->m_TaskMgr.FindActiveTaskByType(712) ||
		localPlayer->m_pIntelligence->m_TaskMgr.FindActiveTaskByType(718) ||
		localPlayer->m_pIntelligence->m_TaskMgr.FindActiveTaskByType(800))
		return;

	CPad* pad = localPlayer->GetPadFromPlayer();

	if (!pad->OldState.DPadUp && pad->NewState.DPadUp) // G key
	{
		CVehicle* minVehicle = nullptr;
		float minDistance = 99999999.0f;

		for (auto vehicle : CPools::ms_pVehiclePool)
		{
			float length = (vehicle->m_matrix->pos - localPlayer->m_matrix->pos).Magnitude();
			
			if (length < minDistance)
			{
				minVehicle = vehicle;
				minDistance = length;
			}
		}

		if (minDistance <= 8.0f)
		{
			int doorId = 0;
			CVector temp; // not checked by nullptr in the game, so we should use temporary var

			if (CCarEnterExit::GetNearestCarPassengerDoor(localPlayer, minVehicle, &temp, &doorId, true, true, true))
			{
				CTaskComplexEnterCarAsPassenger* task = new CTaskComplexEnterCarAsPassenger(minVehicle, doorId, false);
				localPlayer->m_pIntelligence->m_TaskMgr.SetTask(task, 3, false);

				CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(minVehicle);
				CPackets::VehicleEnter packet{};

				packet.vehicleid = vehicle->m_nVehicleId;
				packet.seatid = CCarEnterExit::ComputePassengerIndexFromCarDoor(minVehicle, doorId);
				packet.force = false;
				packet.passenger = true;

				CNetwork::SendPacket(CPacketsID::VEHICLE_ENTER, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
			}
		}
	}
}
