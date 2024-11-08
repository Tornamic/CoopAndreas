#include "CNetworkPed.h"
#include "../../CLocalPlayer.h"
#include "../../CPackets.h"
#include "../../CNetwork.h"
#include "../../CUtil.h"

// CREATE new ped!!! NOT GET!! 
/*CNetworkPed::CNetworkPed(CPed* ped)
{
    if (!CLocalPlayer::m_bIsHost)
        return;

    m_pPed = ped;
    m_nPedId = CNetworkPedManager::GetFreeID();
    m_nCreatedBy = ped->m_nCreatedBy;

    CPackets::PedSpawn packet{};
    packet.pedid = m_nPedId;
    packet.modelId = ped->m_nModelIndex;
    packet.pos = ped->m_matrix->pos;
    packet.pedType = ped->m_nPedType;
    packet.createdBy = ped->m_nCreatedBy;
    CNetwork::SendPacket(ePacketType::PED_SPAWN, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
}*/

CNetworkPed::~CNetworkPed()
{
    if (CLocalPlayer::m_bIsHost)
    {
        CPackets::PedRemove packet{};
        packet.m_nPedId = this->GetId();
        CNetwork::SendPacket(ePacketType::PED_REMOVE, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    }
    this->StreamOut();
}

void CNetworkPed::Create()
{
    int16_t modelId = this->GetModelId();

    CStreaming::RequestModel(modelId, 0);
    CStreaming::LoadAllRequestedModels(false);

    if (this->m_nPedType == PED_TYPE_COP)
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

    if (this->m_nPedType == PED_TYPE_COP)
    {
        this->m_pEntity = new CCopPed((eCopType)modelId);
    }
    else if (this->m_nPedType == PED_TYPE_MEDIC || this->m_nPedType == PED_TYPE_FIREMAN)
    {
        this->m_pEntity = new CEmergencyPed(this->m_nPedType, modelId);
    }
    else
    {
        this->m_pEntity = new CCivilianPed(this->m_nPedType, modelId);
    }

    m_pEntity->m_nCreatedBy = 2;
    m_pEntity->m_pIntelligence->SetPedDecisionMakerType(-1);
    m_pEntity->m_pIntelligence->SetSeeingRange(30.0);
    m_pEntity->m_pIntelligence->SetHearingRange(30.0);
    m_pEntity->m_pIntelligence->m_fDmRadius = 0.0f;
    m_pEntity->m_pIntelligence->m_nDmNumPedsToScan = 0;

    auto& syncData = this->GetSyncData();

    m_pEntity->SetPosn(syncData.m_vecPosition);
    m_pEntity->SetOrientation(0.f, 0.f, 0.f);
    m_pEntity->m_vecMoveSpeed = syncData.m_vecVelocity;
    m_pEntity->m_fHealth = syncData.m_nHealth;
    m_pEntity->m_fArmour = syncData.m_nArmour;
    CUtil::GiveWeaponByPacket(this, syncData.m_nCurrentWeapon, syncData.m_nAmmoInClip);
    m_pEntity->m_fAimingRotation = syncData.m_fAimingRotation;
    m_pEntity->m_fCurrentRotation = syncData.m_fCurrentRotation;
    m_pEntity->field_73C = syncData.m_fLookDirection;
    m_pEntity->m_nMoveState = syncData.m_nMoveState;

    CWorld::Add(m_pEntity);
}

void CNetworkPed::Destroy()
{
    CTheScripts::RemoveThisPed(this->m_pEntity);
}

void CNetworkPed::StreamIn()
{
    if (this->GetEntity() == nullptr)
    {
        this->Create();
    }
}

void CNetworkPed::StreamOut()
{
    if (this->GetEntity())
    {
        this->Destroy();
    }
}
