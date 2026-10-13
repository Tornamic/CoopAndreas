#include "stdafx.h"

CNetworkPlayer::CNetworkPlayer(ENetPeer* peer, playerid_t playerid)
{
    m_pPeer = peer;
    m_playerId = playerid;
}

std::string CNetworkPlayer::GetName()
{
    if (m_Name[0])
        return std::string(m_Name);
    else
        return "player " + std::to_string(m_playerId.value());
}

void CNetworkPlayer::RemoveFromVehicle()
{
    if (this->m_nSeatId < 0 || this->m_nSeatId > 7)
        return;

    if (auto vehicle = CNetworkVehicleManager::GetVehicle(this->m_vehicleId))
    {
        if (vehicle->m_pPlayers[this->m_nSeatId] == this)
        {
            vehicle->SetOccupant(this->m_nSeatId, nullptr);
        }
    }

    this->m_vehicleId = vehicleid_t();
    this->m_nSeatId = -1;
}
