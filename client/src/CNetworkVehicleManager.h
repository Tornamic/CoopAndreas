#pragma once
class CNetworkVehicleManager
{
public:
    static inline std::vector<CNetworkVehicle*> m_pVehicles;

    static inline uint32_t ms_nNextVehicleId = 1;

    static CNetworkVehicle* GetVehicle(vehicleid_t vehicleid);
    static CNetworkVehicle* GetVehicle(CEntity* vehicle);
    static void Add(CNetworkVehicle* vehicle);
    static void Remove(CNetworkVehicle* vehicle);
    static void UpdateDriver(CVehicle* pVehicle);
    static void UpdateIdle();
    static void UpdatePassenger(CVehicle* pVehicle, CPlayerPed* pPlayerPed);
    static vehicleid_t AllocateVehicleId();
    static void ResetIdAllocator();
    static void RemoveHostedUnused();
    static void UpdateDamageSync();
    static vehicleid_t GetTrailerId(CVehicle* pVehicle);
    static void ApplyTrailer(CVehicle* pVehicle, vehicleid_t trailerid);
    static void GetTrainSnapshot(CTrain* pTrain, Packets::Vehicles::STrainSnapshot& snapshot);
    static void ApplyTrainSnapshot(CTrain* pTrain, const Packets::Vehicles::STrainSnapshot& snapshot);
    static void DerailTrain(CTrain* pTrain);
    static bool IsTrainOnRails(CVehicle* pVehicle);
};

