
#include "CVehicle.h"
#include "CVehicleManager.h"

CVehicle::CVehicle(int vehicleid, unsigned short model, CVector pos, float rot)
{
	m_nVehicleId = vehicleid;
	m_nModelId = model;
	m_vecPosition = pos;
	m_vecRotation = CVector( 0, 0, 0 );
}

void CVehicle::ReassignSyncer(CPlayer* newSyncer)
{
	if (this->m_pSyncer != newSyncer)
	{
		CVehiclePackets::AssignVehicleSyncer assignVehicleSyncerPacket{};
		assignVehicleSyncerPacket.vehicleid = this->m_nVehicleId;

		// send to the old vehicle syncer
		if (this->m_pSyncer)
		{
			CNetwork::SendPacket(this->m_pSyncer->m_pPeer, CPacketsID::ASSIGN_VEHICLE, &assignVehicleSyncerPacket, sizeof(assignVehicleSyncerPacket), ENET_PACKET_FLAG_RELIABLE);
		}

		// send to the new vehicle syncer
		CNetwork::SendPacket(newSyncer->m_pPeer, CPacketsID::ASSIGN_VEHICLE, &assignVehicleSyncerPacket, sizeof(assignVehicleSyncerPacket), ENET_PACKET_FLAG_RELIABLE);

		this->m_pSyncer = newSyncer; // update the vehicle syncer
	}
}

void CVehicle::SetOccupant(uint8_t seatid, CPlayer* player)
{
	if (seatid < 0 || seatid > 7)
		return;

	if (this->m_pPlayers[seatid])
	{
		this->m_pPlayers[seatid]->m_nVehicleId = -1;
		this->m_pPlayers[seatid]->m_nSeatId = -1;
	}

	this->m_pPlayers[seatid] = player;

	if (player)
	{
		player->m_nVehicleId = this->m_nVehicleId;
		player->m_nSeatId = seatid;
	}
}
