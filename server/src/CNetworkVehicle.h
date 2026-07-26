#pragma once

#include <iostream>
#include <vector>
#include <algorithm>

#include "CVector.h"
#include "CNetworkPlayerManager.h"
#include <CDamageManager.h>

class CNetworkVehicle
{
public:
    CNetworkVehicle(int vehicleid, unsigned short model, CVector pos, float rot);

    int m_nVehicleId;
    CNetworkPlayer* m_pSyncer = nullptr;
    unsigned short m_nModelId;
    CVector m_vecPosition;
    CVector m_vecRotation;
    CVector m_vecVelocity;
    CNetworkPlayer* m_pPlayers[8] = {nullptr};
    unsigned char m_nPrimaryColor;
    unsigned char m_nSecondaryColor;
    CDamageManager m_damageManager;
    std::vector<int> m_pComponents;
    uint8_t m_nCreatedBy;
    bool m_bUsedByPed = false;

    void ReassignSyncer(CNetworkPlayer* newSyncer);
    void SetOccupant(int8_t seatid, CNetworkPlayer* player);

    ~CNetworkVehicle() {}
};