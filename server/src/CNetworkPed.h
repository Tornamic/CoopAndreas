#pragma once

#include "CVector.h"
#include "network/ids.h"

#include <eModelID.h>
#include <ePedType.h>

class CNetworkPlayer;

class CNetworkPed
{
public:
    CNetworkPed(
        pedid_t pedid, CNetworkPlayer* syncer, eModelID modelId, ePedType pedType, CVector pos, eCharCreatedBy createdBy);

    pedid_t m_pedId;
    CNetworkPlayer* m_pSyncer;
    eModelID m_nModelId;
    ePedType m_nPedType;
    CVector m_vecPos;
    eCharCreatedBy m_nCreatedBy;
    char m_szSpecialModelName[8];
};
