#pragma once

#include "network/packets/objects.h"

class CNetworkObject
{
public:
    CNetworkObject(const Packets::Objects::ObjectSpawn& packet);

    objectid_t m_objectId;
    CNetworkPlayer* m_pSyncer = nullptr;
    unsigned short m_nModelId;
    CVector m_vecPosition;
    CVector m_vecRight;
    CVector m_vecUp;
    uint8_t m_nAreaCode;
    Packets::Objects::SObjectDamageState m_damageState;

    Packets::Objects::ObjectUpdate m_lastUpdate{};
    bool m_bHasUpdate = false;

    ~CNetworkObject() {}
};
