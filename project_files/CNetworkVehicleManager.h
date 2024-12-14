#pragma once
class CNetworkVehicleManager
{
public:
    static std::vector<CNetworkVehicle*> m_pVehicles;
    static CNetworkVehicle* m_apTempVehicles[255];

    static CNetworkVehicle* GetVehicle(int vehicleid);
    static CNetworkVehicle* GetVehicle(CVehicle* vehicle);
    static CNetworkVehicle* GetVehicle(CEntity* vehicle);
    static void Add(CNetworkVehicle* vehicle);
    static void Remove(CNetworkVehicle* vehicle);
    static void UpdateDriver(CVehicle* vehicle);
    static void UpdateIdle();
    static void UpdatePassenger(CVehicle* vehicle, CPlayerPed* localPlayer);
    static unsigned char AddToTempList(CNetworkVehicle* networkVehicle);
};

