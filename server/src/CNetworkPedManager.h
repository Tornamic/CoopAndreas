#pragma once

#include <vector>
#include "network/ids.h"

class CNetworkPlayer;
class CNetworkPed;

class CNetworkPedManager
{
public:
    static std::vector<CNetworkPed*> m_pPeds;
    static void Add(CNetworkPed* ped);
    static void Remove(CNetworkPed* ped);
    static CNetworkPed* GetPed(pedid_t pedid);
    static void RemoveAllHostedAndNotify(CNetworkPlayer* player);
};