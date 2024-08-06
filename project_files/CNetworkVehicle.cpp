#include "stdafx.h"

int m_nVehicleId = -1;
CVehicle* m_pVehicle = nullptr;

// CREATE new vehicle!!! NOT GET!! 
CNetworkVehicle::CNetworkVehicle(CVehicle* vehicle)
{
    if (!CLocalPlayer::m_bIsHost)
        return;
    
    m_pVehicle = vehicle;
    m_nVehicleId = CNetworkVehicleManager::GetFreeID();
    
    CPackets::VehicleSpawn vehicleSpawnPacket{};
    vehicleSpawnPacket.vehicleid = m_nVehicleId;
    vehicleSpawnPacket.modelid = vehicle->m_nModelIndex;
    vehicleSpawnPacket.pos = vehicle->m_matrix->pos;
    vehicleSpawnPacket.rot = vehicle->GetHeading();
    CNetwork::SendPacket(CPacketsID::VEHICLE_SPAWN, &vehicleSpawnPacket, sizeof vehicleSpawnPacket, ENET_PACKET_FLAG_RELIABLE);

}

CNetworkVehicle::CNetworkVehicle(int vehicleid, int modelid, CVector pos, float rotation)
{
    unsigned char oldFlags = CStreaming::ms_aInfoForModel[modelid].m_nFlags;
    CStreaming::RequestModel(modelid, GAME_REQUIRED);
    CStreaming::LoadAllRequestedModels(false);

    if (CStreaming::ms_aInfoForModel[modelid].m_nLoadState != LOADSTATE_LOADED)
        return;

    if (!(oldFlags & GAME_REQUIRED)) 
    {
        CStreaming::SetModelIsDeletable(modelid);
        CStreaming::SetModelTxdIsDeletable(modelid);
    }

    switch (((CVehicleModelInfo*)CModelInfo::ms_modelInfoPtrs[modelid])->m_nVehicleType)
    {
        case VEHICLE_MTRUCK:
            m_pVehicle = new CMonsterTruck(modelid, 1); break;

        case VEHICLE_QUAD:
            m_pVehicle = new CQuadBike(modelid, 1); break;

        case VEHICLE_HELI:
            m_pVehicle = new CHeli(modelid, 1); break;

        case VEHICLE_PLANE:
            m_pVehicle = new CPlane(modelid, 1); break;

        case VEHICLE_BIKE:
            m_pVehicle = new CBike(modelid, 1);
            ((CBike*)m_pVehicle)->m_nDamageFlags |= 0x10; break;

        case VEHICLE_BMX:
            m_pVehicle = new CBmx(modelid, 1);
            ((CBmx*)m_pVehicle)->m_nDamageFlags |= 0x10; break;

        case VEHICLE_TRAILER:
            m_pVehicle = new CTrailer(modelid, 1); break;

        case VEHICLE_BOAT:
            m_pVehicle = new CBoat(modelid, 1); break;

        default:
            m_pVehicle = new CAutomobile(modelid, 1, true); break;
    }

    if (!m_pVehicle)
        return;

    m_pVehicle->SetPosn(pos);
    m_pVehicle->SetOrientation(0.0f, 0.0f, rotation);
    m_pVehicle->m_nStatus = 4;
    m_pVehicle->m_eDoorLock = DOORLOCK_UNLOCKED;

    CWorld::Add(m_pVehicle);

    if (m_pVehicle->m_nVehicleClass == VEHICLE_BIKE)
        ((CBike*)m_pVehicle)->PlaceOnRoadProperly();
    else if (m_pVehicle->m_nVehicleClass != VEHICLE_BOAT)
        ((CAutomobile*)m_pVehicle)->PlaceOnRoadProperly();

    m_nVehicleId = vehicleid;
}

CNetworkVehicle::~CNetworkVehicle()
{
    if (CLocalPlayer::m_bIsHost)
    {
        CPackets::VehicleRemove vehicleRemovePacket{};
        vehicleRemovePacket.vehicleid = m_nVehicleId;
        CNetwork::SendPacket(CPacketsID::VEHICLE_REMOVE, &vehicleRemovePacket, sizeof vehicleRemovePacket, ENET_PACKET_FLAG_RELIABLE);
    }

    CWorld::Remove(m_pVehicle);
    delete m_pVehicle;
}