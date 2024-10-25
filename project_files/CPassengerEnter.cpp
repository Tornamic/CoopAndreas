#include "stdafx.h"
#include "CNetworkVehicle.h"

void CPassengerEnter::Process()
{
	CPlayerPed* localPlayer = FindPlayerPed(0);

	if (localPlayer == nullptr)
		return;

	if (localPlayer->m_nPedFlags.bInVehicle)
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
			plugin::Command<0x5CA>(CPools::GetPedRef(localPlayer), CPools::GetVehicleRef(minVehicle), 3000, -1);

			CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(minVehicle);
			CPackets::VehicleEnter packet{};
			packet.seatid = -1;
			packet.vehicleid = vehicle->m_nVehicleId;
			packet.force = false;
			CNetwork::SendPacket(CPacketsID::VEHICLE_ENTER, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
		}
	}
}
