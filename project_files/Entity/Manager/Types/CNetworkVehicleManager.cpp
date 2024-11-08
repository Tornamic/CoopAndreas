#include "../../../stdafx.h"
#include "CNetworkVehicleManager.h"
#include "../../../CPackets.h"
#include "../../../CLocalPlayer.h"
#include "../../../CNetwork.h"
#include "../../../CPacketHandler.h"

void CNetworkVehicleManager::UpdateDriver(CVehicle* vehicle)
{
	CNetworkVehicle* networkVehicle = this->Get(vehicle);
	CPackets::VehicleDriverUpdate* packet = CPacketHandler::VehicleDriverUpdate__Collect(networkVehicle);
	CNetwork::SendPacket(ePacketType::VEHICLE_DRIVER_UPDATE, packet, sizeof *packet, ENET_PACKET_FLAG_UNSEQUENCED);
	delete packet;
}

void CNetworkVehicleManager::UpdateIdle()
{
	auto& entities = this->GetEntities();
	for (int i = 0; i != entities.size(); i++)
	{
		if (entities[i]->GetEntity() == nullptr)
			continue;

		if (CLocalPlayer::m_bIsHost && !entities[i]->HasDriver())
		{
			CPackets::VehicleIdleUpdate* packet = CPacketHandler::VehicleIdleUpdate__Collect(entities[i]);
			CNetwork::SendPacket(ePacketType::VEHICLE_IDLE_UPDATE, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED);
			delete packet;
		}
	}
}

void CNetworkVehicleManager::UpdatePassenger(CVehicle* vehicle, CPlayerPed* localPlayer)
{
	CNetworkVehicle* networkVehicle = this->Get(vehicle);
	CPackets::VehiclePassengerUpdate* packet = CPacketHandler::VehiclePassengerUpdate__Collect(networkVehicle, localPlayer);
	CNetwork::SendPacket(ePacketType::VEHICLE_PASSENGER_UPDATE, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED);
	delete packet;
}