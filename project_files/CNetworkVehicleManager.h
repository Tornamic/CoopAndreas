#pragma once
class CNetworkVehicleManager
{
public:
    static std::vector<CNetworkVehicle*> m_pVehicles;

    static CNetworkVehicle* GetVehicle(int vehicleid);
    static CNetworkVehicle* GetVehicle(CVehicle* vehicle);
    static int GetFreeID();
    static void Add(CNetworkVehicle* vehicle);
    static void Remove(CNetworkVehicle* vehicle);
    static void ProcessAll();
};

