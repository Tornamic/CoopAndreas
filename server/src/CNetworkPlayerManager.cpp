#include "stdafx.h"

std::vector<CNetworkPlayer*> CNetworkPlayerManager::m_pPlayers;
uint32_t CNetworkPlayerManager::ms_nNextPlayerId = 1;

void CNetworkPlayerManager::Add(CNetworkPlayer* player)
{
    m_pPlayers.push_back(player);
}

void CNetworkPlayerManager::Remove(CNetworkPlayer* player)
{
    auto it = std::find(m_pPlayers.begin(), m_pPlayers.end(), player);
    if (it != m_pPlayers.end())
    {
        m_pPlayers.erase(it);
    }
}

// find player instance by id
CNetworkPlayer* CNetworkPlayerManager::GetPlayer(playerid_t playerid)
{
    if (!playerid.valid())
        return nullptr;

    for (int i = 0; i != m_pPlayers.size(); i++)
    {
        if (m_pPlayers[i]->m_playerId == playerid)
        {
            return m_pPlayers[i];
        }
    }
    return nullptr;
}

// find player instance by enetpeer
CNetworkPlayer* CNetworkPlayerManager::GetPlayer(ENetPeer* peer)
{
    for (int i = 0; i != m_pPlayers.size(); i++)
    {
        if (m_pPlayers[i]->m_pPeer == peer)
        {
            return m_pPlayers[i];
        }
    }
    return nullptr;
}

// the amount of players is limited by the enet host, not by the id
playerid_t CNetworkPlayerManager::AllocatePlayerId()
{
    return playerid_t(ms_nNextPlayerId++);
}

CNetworkPlayer* CNetworkPlayerManager::GetHost()
{
    for (int i = 0; i != m_pPlayers.size(); i++)
    {
        if (m_pPlayers[i]->m_bIsHost)
        {
            return m_pPlayers[i];
        }
    }
    return nullptr;
}

void CNetworkPlayerManager::AssignHostToFirstPlayer()
{
    if (CNetworkPlayerManager::m_pPlayers.size() <= 0)
        return;

    CNetworkPlayer* player = CNetworkPlayerManager::m_pPlayers.front();
    CNetworkPlayer* host = CNetworkPlayerManager::GetHost();

    if (player == host)
        return;

    if (host != nullptr)
        host->m_bIsHost = false;

    player->m_bIsHost = true;

    Packets::System::PlayerAssignHost playerAssignHost{};
    playerAssignHost.playerid = player->m_playerId;
    GetPacketFactory().SendToAll(playerAssignHost);
}