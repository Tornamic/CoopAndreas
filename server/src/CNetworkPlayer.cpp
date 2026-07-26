#include "stdafx.h"

CNetworkPlayer::CNetworkPlayer(ENetPeer* peer, int playerid)
{
    m_pPeer = peer;
    m_iPlayerId = playerid;
}

std::string CNetworkPlayer::GetName()
{
    if (m_Name[0])
        return std::string(m_Name);
    else
        return "player " + std::to_string(m_iPlayerId);
}

void CNetworkPlayer::RemoveFromVehicle()
{
    if (this->m_nSeatId < 0 || this->m_nSeatId > 7)
        return;

    if (auto vehicle = CNetworkVehicleManager::GetVehicle(this->m_nVehicleId))
    {
        if (vehicle->m_pPlayers[this->m_nSeatId] == this)
        {
            vehicle->SetOccupant(this->m_nSeatId, nullptr);
        }
    }

    this->m_nVehicleId = -1;
    this->m_nSeatId = -1;
}
