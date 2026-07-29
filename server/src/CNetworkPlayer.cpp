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
    if (this->m_nSeatId >= 0 && this->m_nSeatId <= 7)
    {
        if (auto vehicle = CNetworkVehicleManager::GetVehicle(this->m_nVehicleId))
        {
            if (vehicle->m_pPlayers[this->m_nSeatId] == this)
            {
                vehicle->SetOccupant(this->m_nSeatId, nullptr);
            }
        }
    }

    this->m_nVehicleId = -1;
    this->m_nSeatId = -1;
}

void CNetworkPlayer::GuardVehicleSyncFor(uint32_t duration)
{
    m_nVehicleSyncGuardUntil = static_cast<uint32_t>(g_serverTime) + duration;
}

void CNetworkPlayer::ClearVehicleSyncGuard()
{
    m_nVehicleSyncGuardUntil = 0;
}

bool CNetworkPlayer::IsVehicleSyncGuardActive()
{
    if (m_nVehicleSyncGuardUntil == 0)
    {
        return false;
    }

    uint32_t now = static_cast<uint32_t>(g_serverTime);
    if (static_cast<int32_t>(m_nVehicleSyncGuardUntil - now) > 0)
    {
        return true;
    }

    ClearVehicleSyncGuard();
    return false;
}
