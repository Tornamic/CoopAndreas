#include "stdafx.h"
#include <network/packets/vehicles.h>

CNetworkVehicle::CNetworkVehicle(int vehicleid, unsigned short model, CVector pos, float rot)
{
    m_nVehicleId = vehicleid;
    m_nModelId = model;
    m_vecPosition = pos;
    m_vecRotation = CVector(0, 0, 0);
}

void CNetworkVehicle::ReassignSyncer(CNetworkPlayer* newSyncer)
{
    if (m_pSyncer != newSyncer)
    {
        Packets::Vehicles::AssignVehicleSyncer packet{};
        packet.vehicleid = m_nVehicleId;

        // send to the old vehicle syncer
        if (m_pSyncer)
        {
            GetPacketFactory().Send(packet, m_pSyncer);
        }

        // send to the new
        GetPacketFactory().Send(packet, newSyncer);

        m_pSyncer = newSyncer;
    }
}

void CNetworkVehicle::SetOccupant(int8_t seatid, CNetworkPlayer* player)
{
    if (seatid < 0 || seatid > 7)
        return;

    if (player && (player->m_nVehicleId != m_nVehicleId || player->m_nSeatId != seatid))
    {
        player->RemoveFromVehicle();
    }

    if (this->m_pPlayers[seatid] && this->m_pPlayers[seatid] != player)
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
    else
    {
        bool hasOccupants = false;
        for (auto* occupant : m_pPlayers)
        {
            hasOccupants = hasOccupants || occupant != nullptr;
        }

        if (hasOccupants)
        {
            m_nWantedStateSetAt = static_cast<uint32_t>(g_serverTime);
        }
        else
        {
            m_nWantedStateSetAt = 0;
            m_nSharedWantedLevel = 0;
            m_bSharedPoliceEngaged = false;
        }
    }
}
