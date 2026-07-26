#pragma once

#include "CVector.h"

#include <eModelID.h>
#include <ePedType.h>

class CNetworkPlayer;

class CNetworkPed
{
public:
    CNetworkPed(
        int pedid, CNetworkPlayer* syncer, eModelID modelId, ePedType pedType, CVector pos, eCharCreatedBy createdBy);

    int m_nPedId;
    CNetworkPlayer* m_pSyncer;
    eModelID m_nModelId;
    ePedType m_nPedType;
    CVector m_vecPos;
    eCharCreatedBy m_nCreatedBy;
    char m_szSpecialModelName[8];
};
