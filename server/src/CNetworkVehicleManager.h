#pragma once


class CNetworkVehicleManager
{
public:
    static std::vector<CNetworkVehicle*> m_pVehicles;
    static void Add(CNetworkVehicle* vehicle);
    static void Remove(CNetworkVehicle* vehicle);
    static CNetworkVehicle* GetVehicle(vehicleid_t vehicleid);
    static CNetworkVehicle* GetTractor(vehicleid_t trailerid);
    static void RemoveAllHostedAndNotify(CNetworkPlayer* player);
};