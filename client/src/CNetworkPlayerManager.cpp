#include "stdafx.h"

std::vector<CNetworkPlayer*> CNetworkPlayerManager::m_pPlayers;
CPad CNetworkPlayerManager::m_pPads[Config::MAX_SERVER_PLAYERS + 2];
int CNetworkPlayerManager::m_nMyId;

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

CNetworkPlayer* CNetworkPlayerManager::GetPlayer(SenderPlayerId playerid)
{
    return GetPlayer(playerid.value);
}

CNetworkPlayer* CNetworkPlayerManager::GetPlayer(int playerid)
{
    for (int i = 0; i != m_pPlayers.size(); i++)
    {
        if (m_pPlayers[i]->m_iPlayerId == playerid)
        {
            return m_pPlayers[i];
        }
    }
    return nullptr;
}

CNetworkPlayer* CNetworkPlayerManager::GetPlayer(CEntity* entity)
{
    for (int i = 0; i != m_pPlayers.size(); i++)
    {
        if (m_pPlayers[i]->m_pPed == entity)
        {
            return m_pPlayers[i];
        }
    }
    return nullptr;
}