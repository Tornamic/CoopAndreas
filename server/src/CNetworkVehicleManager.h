#pragma once


class CNetworkVehicleManager
{
public:
    static std::vector<CNetworkVehicle*> m_pVehicles;
    static void Add(CNetworkVehicle* vehicle);
    static void Remove(CNetworkVehicle* vehicle);
    static CNetworkVehicle* GetVehicle(int vehicleid);
    static int GetFreeID();
    static void RemoveAllHostedAndNotify(CNetworkPlayer* player);
};