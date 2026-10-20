#include "stdafx.h"

CNetworkObject::CNetworkObject(const Packets::Objects::ObjectSpawn& packet)
{
    m_objectId = packet.objectid;
    m_nModelId = packet.modelid;
    m_vecPosition = packet.pos;
    m_vecRight = packet.right;
    m_vecUp = packet.up;
    m_nAreaCode = packet.areaCode;
    m_damageState = packet.damageState;
}
