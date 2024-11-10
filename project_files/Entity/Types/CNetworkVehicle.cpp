#include "../../stdafx.h"
#include "CNetworkVehicle.h"
#include "../../CLocalPlayer.h"
#include "../Manager/Types/CNetworkVehicleManager.h"
#include "../../CPackets.h"
#include "../../CNetwork.h"
#include "../../CUtil.h"

// CREATE new vehicle!!! NOT GET!! 
/*CNetworkVehicle::CNetworkVehicle(CVehicle* vehicle)
{
    if (!CLocalPlayer::m_bIsHost)
        return;
    
    m_pVehicle = vehicle;
    m_nVehicleId = CNetworkVehicleManager::GetFreeID();
    
    CPackets::VehicleSpawn vehicleSpawnPacket{};
    vehicleSpawnPacket.vehicleid = m_nVehicleId;
    vehicleSpawnPacket.modelId = vehicle->m_nModelIndex;
    vehicleSpawnPacket.pos = vehicle->m_matrix->pos;
    vehicleSpawnPacket.rot = vehicle->GetHeading();
    vehicleSpawnPacket.color1 = vehicle->m_nPrimaryColor;
    vehicleSpawnPacket.color2 = vehicle->m_nSecondaryColor;
    CNetwork::SendPacket(ePacketType::VEHICLE_SPAWN, &vehicleSpawnPacket, sizeof vehicleSpawnPacket, ENET_PACKET_FLAG_RELIABLE);

}*/

CNetworkVehicle::~CNetworkVehicle()
{
    if (CLocalPlayer::m_bIsHost)
    {
        CPackets::VehicleRemove vehicleRemovePacket{};
        vehicleRemovePacket.m_nVehicleId = this->GetId();
        CNetwork::SendPacket(ePacketType::VEHICLE_REMOVE, &vehicleRemovePacket, sizeof vehicleRemovePacket, ENET_PACKET_FLAG_RELIABLE);
    }
    else
    {
        this->Destroy();
    }
}

CNetworkVehicle* CNetworkVehicle::NotifyNew(CVehicle* vehicle)
{
    uint16_t freeId = CNetworkVehicleManager::Instance().GetFreeId();

    if (freeId == -1)
        return nullptr;

    CPackets::VehicleSpawn vehicleSpawnPacket{};
    vehicleSpawnPacket.m_nVehicleId = freeId;
    vehicleSpawnPacket.m_nModelId = vehicle->m_nModelIndex;
    vehicleSpawnPacket.m_vecPosition = vehicle->m_matrix->pos;
    vehicleSpawnPacket.m_fRotation = vehicle->GetHeading();
    vehicleSpawnPacket.m_nPrimaryColor = vehicle->m_nPrimaryColor;
    vehicleSpawnPacket.m_nSecondaryColor = vehicle->m_nSecondaryColor;
    CNetwork::SendPacket(ePacketType::VEHICLE_SPAWN, &vehicleSpawnPacket, sizeof vehicleSpawnPacket, ENET_PACKET_FLAG_RELIABLE);

    return new CNetworkVehicle(freeId, vehicle->m_nModelIndex);
}

void CNetworkVehicle::Create()
{
    int16_t modelId = this->GetModelId();

    uint8_t oldFlags = CStreaming::ms_aInfoForModel[modelId].m_nFlags;
    CStreaming::RequestModel(modelId, GAME_REQUIRED | PRIORITY_REQUEST);
    CStreaming::LoadAllRequestedModels(true);

    if (!(oldFlags & GAME_REQUIRED))
    {
        CStreaming::SetModelIsDeletable(modelId);
        CStreaming::SetModelTxdIsDeletable(modelId);
    }

    switch (((CVehicleModelInfo*)CModelInfo::ms_modelInfoPtrs[modelId])->m_nVehicleType)
    {
    case VEHICLE_MTRUCK:
        m_pEntity = new CMonsterTruck(modelId, eVehicleCreatedBy::MISSION_VEHICLE); break;

    case VEHICLE_QUAD:
        m_pEntity = new CQuadBike(modelId, eVehicleCreatedBy::MISSION_VEHICLE); break;

    case VEHICLE_HELI:
        m_pEntity = new CHeli(modelId, eVehicleCreatedBy::MISSION_VEHICLE); break;

    case VEHICLE_PLANE:
        m_pEntity = new CPlane(modelId, eVehicleCreatedBy::MISSION_VEHICLE); break;

    case VEHICLE_BIKE:
        m_pEntity = new CBike(modelId, eVehicleCreatedBy::MISSION_VEHICLE);
        ((CBike*)m_pEntity)->m_nDamageFlags |= 0x10; break;

    case VEHICLE_BMX:
        m_pEntity = new CBmx(modelId, eVehicleCreatedBy::MISSION_VEHICLE);
        ((CBmx*)m_pEntity)->m_nDamageFlags |= 0x10; break;

    case VEHICLE_TRAILER:
        m_pEntity = new CTrailer(modelId, eVehicleCreatedBy::MISSION_VEHICLE); break;

    case VEHICLE_BOAT:
        m_pEntity = new CBoat(modelId, eVehicleCreatedBy::MISSION_VEHICLE); break;

    case VEHICLE_TRAIN:
        return;

    default:
        m_pEntity = new CAutomobile(modelId, 1, true); break;
    }

    if (!m_pEntity)
        return;

    auto& syncData = this->GetSyncData();

    m_pEntity->SetPosn(syncData.m_vecPosition);
    m_pEntity->SetOrientation(syncData.m_vecRotation.x, syncData.m_vecRotation.y, syncData.m_vecRotation.z);
    m_pEntity->m_nStatus = 4;
    m_pEntity->m_eDoorLock = (eDoorLock)syncData.m_nDoorLock;
    m_pEntity->m_nPrimaryColor = syncData.m_nPrimaryColor;
    m_pEntity->m_nSecondaryColor = syncData.m_nSecondaryColor;

    CWorld::Add(m_pEntity);
}

void CNetworkVehicle::Destroy()
{
    if (m_pEntity)
    {
        CWorld::Remove(m_pEntity);
        m_pEntity->~CVehicle();
    }
}

bool CNetworkVehicle::HasDriver()
{
    if (m_pEntity == nullptr)
        return false;

    return m_pEntity->m_pDriver != nullptr;
}

void CNetworkVehicle::StreamIn()
{
    if (this->GetEntity() == nullptr)
    {
        this->Create();
    }

    auto vehicle = this->GetEntity();
    auto& syncData = this->GetSyncData();

    vehicle->m_vecMoveSpeed = syncData.m_vecVelocity;
    vehicle->m_matrix->up = syncData.m_vecRotation;
    vehicle->m_matrix->right = syncData.m_vecRoll;
    vehicle->m_fHealth = syncData.m_nHealth;
    vehicle->SetRemap(syncData.m_nPaintjob);

    eVehicleType vehicleType = CUtil::GetVehicleType(vehicle);

    if (vehicleType == VEHICLE_BIKE || vehicleType == VEHICLE_BMX)
    {
        ((CBike*)vehicle)->m_rideAnimData.m_fAnimLean = syncData.m_fBikeLean;
    }
    else if (vehicleType == VEHICLE_PLANE)
    {
        CPlane* plane = (CPlane*)vehicle;
        plane->m_fLandingGearStatus = syncData.m_fPlaneGearState;

        if (this->GetModelId() == MODEL_HYDRA)
        {
            plane->m_wMiscComponentAngle = syncData.m_nMiscComponentAngle;
        }
    }
}

void CNetworkVehicle::StreamOut()
{
    if (this->GetEntity())
    {
        this->Destroy();
    }
}