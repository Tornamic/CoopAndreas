#pragma once
class CNetworkVehicleManager
{
public:
    static inline std::vector<CNetworkVehicle*> m_pVehicles;
    static inline CNetworkVehicle* m_apTempVehicles[255];

    static CNetworkVehicle* GetVehicle(int vehicleid);
    static CNetworkVehicle* GetVehicle(CEntity* vehicle);
    static void Add(CNetworkVehicle* vehicle);
    static void Remove(CNetworkVehicle* vehicle);
    static void UpdateDriver(CVehicle* pVehicle);
    static void UpdateIdle();
    static void UpdatePassenger(CVehicle* pVehicle, CPlayerPed* pPlayerPed);
    static uint8_t AddToTempList(CNetworkVehicle* networkVehicle);
    static void RemoveHostedUnused();
    static void UpdateDamageSync();
};

