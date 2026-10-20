#pragma once

class CNetworkObjectManager
{
public:
    static std::vector<CNetworkObject*> m_pObjects;
    static void Add(CNetworkObject* object);
    static void Remove(CNetworkObject* object);
    static CNetworkObject* GetObject(objectid_t objectid);
    static void RemoveAllHostedAndNotify(CNetworkPlayer* player);
};
