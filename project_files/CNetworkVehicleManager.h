#pragma once
class CNetworkVehicleManager
{
public:
    static std::vector<CNetworkVehicle*> m_pVehicles;

    static CNetworkVehicle* GetVehicle(int vehicleid);
    static CNetworkVehicle* GetVehicle(CVehicle* vehicle);
    static CNetworkVehicle* GetVehicle(CEntity* vehicle);
    static int GetFreeID();
    static void Add(CNetworkVehicle* vehicle);
    static void Remove(CNetworkVehicle* vehicle);
    static void UpdateDriver(CVehicle* vehicle);
    static void UpdateIdle();
    static void UpdatePassenger(CVehicle* vehicle, CPlayerPed* localPlayer);
};

