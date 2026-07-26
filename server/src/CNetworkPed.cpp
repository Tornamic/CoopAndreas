#include "stdafx.h"

CNetworkPed::CNetworkPed(
    int pedid, CNetworkPlayer* syncer, eModelID modelId, ePedType pedType, CVector pos, eCharCreatedBy createdBy)
{
    m_nPedId = pedid;
    m_pSyncer = syncer;
    m_nModelId = modelId;
    m_nPedType = pedType;
    m_vecPos = pos;
    m_nCreatedBy = createdBy;
}
