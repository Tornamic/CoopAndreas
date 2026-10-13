#include "stdafx.h"
#include <network/packets/peds.h>

std::vector<CNetworkPed*> CNetworkPedManager::m_pPeds;

void CNetworkPedManager::Add(CNetworkPed* ped)
{
    m_pPeds.push_back(ped);
}

void CNetworkPedManager::Remove(CNetworkPed* ped)
{
    auto it = std::find(m_pPeds.begin(), m_pPeds.end(), ped);
    // std::find()
    if (it != m_pPeds.end())
    {
        m_pPeds.erase(it);
    }
}

CNetworkPed* CNetworkPedManager::GetPed(pedid_t pedid)
{
    if (!pedid.valid())
        return nullptr;

    for (int i = 0; i != m_pPeds.size(); i++)
    {
        if (m_pPeds[i]->m_pedId == pedid)
        {
            return m_pPeds[i];
        }
    }
    return nullptr;
}

void CNetworkPedManager::RemoveAllHostedAndNotify(CNetworkPlayer* player)
{
    Packets::Peds::PedRemove packet{};

    for (auto it = CNetworkPedManager::m_pPeds.begin(); it != CNetworkPedManager::m_pPeds.end();)
    {
        if ((*it)->m_pSyncer == player)
        {
            packet.pedid = (*it)->m_pedId;
            GetPacketFactory().SendToAll(packet, player);
            delete *it;
            it = CNetworkPedManager::m_pPeds.erase(it);
        }
        else
        {
            ++it;
        }
    }
}