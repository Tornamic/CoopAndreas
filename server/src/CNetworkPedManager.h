#pragma once

#include <vector>

class CNetworkPlayer;
class CNetworkPed;

class CNetworkPedManager
{
public:
    static std::vector<CNetworkPed*> m_pPeds;
    static void Add(CNetworkPed* ped);
    static void Remove(CNetworkPed* ped);
    static CNetworkPed* GetPed(int pedid);
    static int GetFreeId();
    static void RemoveAllHostedAndNotify(CNetworkPlayer* player);
};