#include "stdafx.h"

// CREATE new ped!!! NOT GET!! 
CNetworkPed::CNetworkPed(CPed* ped)
{
    if (!CLocalPlayer::m_bIsHost)
        return;

    m_pPed = ped;
    m_nPedId = CNetworkPedManager::GetFreeID();

    CPackets::PedSpawn packet{};
    packet.pedid = m_nPedId;
    packet.modelId = ped->m_nModelIndex;
    packet.pos = ped->m_matrix->pos;
    packet.pedType = ped->m_nPedType;
    packet.createdBy = ped->m_nCreatedBy;
    CNetwork::SendPacket(CPacketsID::PED_SPAWN, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
}

CNetworkPed::CNetworkPed(int pedid, int modelId, ePedType pedType, CVector pos, unsigned char createdBy)
{
    CStreaming::RequestModel(modelId, 0);
    CStreaming::LoadAllRequestedModels(false);

    if (pedType == PED_TYPE_COP)
    {
        switch (modelId) 
        {
        case MODEL_LAPD1:
        case MODEL_SFPD1:
        case MODEL_LVPD1:
        case MODEL_LAPDM1:
            modelId = COP_TYPE_CITYCOP;
            break;
        case MODEL_CSHER:
            modelId = COP_TYPE_CSHER;
            break;
        case MODEL_SWAT:
            modelId = COP_TYPE_SWAT1;
            break;
        case MODEL_FBI:
            modelId = COP_TYPE_FBI;
            break;
        case MODEL_ARMY:
            modelId = COP_TYPE_ARMY;
            break;
        }
    }

    if (pedType == PED_TYPE_COP)
    {
        m_pPed = new CCopPed((eCopType)modelId);
    }
    else if (pedType == PED_TYPE_MEDIC || pedType == PED_TYPE_FIREMAN)
    {
        m_pPed = new CEmergencyPed(pedType, modelId);
    }
    else
    {
        m_pPed = new CCivilianPed(pedType, modelId);
    }

    m_pPed->SetCharCreatedBy(createdBy);

    m_pPed->SetPosn(pos);
    m_pPed->SetOrientation(0.f, 0.f, 0.f);
    CWorld::Add(m_pPed);

    m_pPed->m_pIntelligence->m_TaskMgr.SetTask(new CTaskComplexWanderStandard(4, 0, true), 4, false);

    m_nPedId = pedid;
    m_nPedType = pedType;
}

CNetworkPed::~CNetworkPed()
{
    if (CLocalPlayer::m_bIsHost)
    {
        CPackets::PedRemove packet{};
        packet.pedid = m_nPedId;
        CNetwork::SendPacket(CPacketsID::PED_REMOVE, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    }
    else
    {
        if (m_pPed && m_pPed->m_matrix->m_pOwner)
        {
            CWorld::Remove(m_pPed);
            //CWorld::RemoveReferencesToDeletedObject(m_pPed);
            m_pPed->Remove();
            delete m_pPed;
        }
    }
}