#include "stdafx.h"
#include "CNetworkVehicle.h"

CNetworkVehicle::CNetworkVehicle(int vehicleid, int modelid, CVector pos, float rotation, unsigned char color1, unsigned char color2, unsigned char createdBy)
{
    if (auto vehicle = CNetworkVehicleManager::GetVehicle(vehicleid))
    {
        if (vehicle->m_pVehicle && CUtil::IsValidEntityPtr(vehicle->m_pVehicle))
        {
            CWorld::Remove(vehicle->m_pVehicle);
            delete vehicle->m_pVehicle;
        }
        CNetworkVehicleManager::Remove(vehicle);
    }

    m_nVehicleId = vehicleid;
    m_bSyncing = false;
    m_nTempId = 255;
    m_nModelId = modelid;
    m_nCreatedBy = createdBy;

    if (!CreateVehicle(vehicleid, modelid, pos, rotation, color1, color2))
        return;

}

bool CNetworkVehicle::CreateVehicle(int vehicleid, int modelid, CVector pos, float rotation, unsigned char color1, unsigned char color2)
{
    unsigned char oldFlags = CStreaming::ms_aInfoForModel[modelid].m_nFlags;
    CStreaming::RequestModel(modelid, GAME_REQUIRED);
    CStreaming::LoadAllRequestedModels(false);

    if (!(oldFlags & GAME_REQUIRED))
    {
        CStreaming::SetModelIsDeletable(modelid);
        CStreaming::SetModelTxdIsDeletable(modelid);
    }

    switch (((CVehicleModelInfo*)CModelInfo::ms_modelInfoPtrs[modelid])->m_nVehicleType)
    {
    case VEHICLE_MTRUCK:
        m_pVehicle = new CMonsterTruck(modelid, MISSION_VEHICLE); break;

    case VEHICLE_QUAD:
        m_pVehicle = new CQuadBike(modelid, MISSION_VEHICLE); break;

    case VEHICLE_HELI:
        m_pVehicle = new CHeli(modelid, MISSION_VEHICLE); break;

    case VEHICLE_PLANE:
        m_pVehicle = new CPlane(modelid, MISSION_VEHICLE); break;

    case VEHICLE_BIKE:
        m_pVehicle = new CBike(modelid, MISSION_VEHICLE);
        ((CBike*)m_pVehicle)->m_nDamageFlags |= 0x10; break;

    case VEHICLE_BMX:
        m_pVehicle = new CBmx(modelid, MISSION_VEHICLE);
        ((CBmx*)m_pVehicle)->m_nDamageFlags |= 0x10; break;

    case VEHICLE_TRAILER:
        m_pVehicle = new CTrailer(modelid, MISSION_VEHICLE); break;

    case VEHICLE_BOAT:
        m_pVehicle = new CBoat(modelid, MISSION_VEHICLE); break;

    case VEHICLE_TRAIN:
        return false;

    default:
        m_pVehicle = new CAutomobile(modelid, MISSION_VEHICLE, true); break;
    }

    if (!m_pVehicle)
        return false;

    m_pVehicle->SetPosn(pos);
    m_pVehicle->SetOrientation(0.0f, 0.0f, rotation);
    m_pVehicle->m_nStatus = 4;
    m_pVehicle->m_eDoorLock = DOORLOCK_UNLOCKED;
    m_pVehicle->m_nPrimaryColor = color1;
    m_pVehicle->m_nSecondaryColor = color2;
    CWorld::Add(m_pVehicle);

    return true;
}

CNetworkVehicle::~CNetworkVehicle()
{
    if (m_bSyncing)
    {
        CPackets::VehicleRemove vehicleRemovePacket{};
        vehicleRemovePacket.vehicleid = m_nVehicleId;
        CNetwork::SendPacket(CPacketsID::VEHICLE_REMOVE, &vehicleRemovePacket, sizeof vehicleRemovePacket, ENET_PACKET_FLAG_RELIABLE);
    }
    else
    {
        if (m_pVehicle && CUtil::IsValidEntityPtr(m_pVehicle))
        {
            if (m_nBlipHandle != -1)
            {
                CRadar::ClearBlipForEntity(eBlipType::BLIP_CAR, CPools::GetVehicleRef(m_pVehicle));
            }
            plugin::Command<Commands::DELETE_CAR>(CPools::GetVehicleRef(m_pVehicle));
        }
    }
}

bool CNetworkVehicle::HasDriver()
{
    if (m_pVehicle == nullptr)
        return false;

    return m_pVehicle->m_pDriver != nullptr;
}

CNetworkVehicle* CNetworkVehicle::CreateHosted(CVehicle* vehicle)
{
    vehicle->m_nTimeTillWeNeedThisCar += 5000;

    CNetworkVehicle* networkVehicle = new CNetworkVehicle();

    networkVehicle->m_pVehicle = vehicle;
    networkVehicle->m_nVehicleId = -1;
    networkVehicle->m_bSyncing = true;
    networkVehicle->m_nModelId = vehicle->m_nModelIndex;
    networkVehicle->m_nPaintJob = (char)vehicle->m_nRemapTxd;
    networkVehicle->m_nTempId = CNetworkVehicleManager::AddToTempList(networkVehicle);
    networkVehicle->m_nCreatedBy = vehicle->m_nCreatedBy;

    CPackets::VehicleSpawn vehicleSpawnPacket{};
    vehicleSpawnPacket.vehicleid = -1;
    vehicleSpawnPacket.tempid = networkVehicle->m_nTempId;
    vehicleSpawnPacket.modelid = vehicle->m_nModelIndex;
    vehicleSpawnPacket.pos = vehicle->m_matrix->pos;
    vehicleSpawnPacket.rot = vehicle->GetHeading();
    vehicleSpawnPacket.color1 = vehicle->m_nPrimaryColor;
    vehicleSpawnPacket.color2 = vehicle->m_nSecondaryColor;
    vehicleSpawnPacket.createdBy = vehicle->m_nCreatedBy;
    CNetwork::SendPacket(CPacketsID::VEHICLE_SPAWN, &vehicleSpawnPacket, sizeof vehicleSpawnPacket, ENET_PACKET_FLAG_RELIABLE);

    return networkVehicle;
}