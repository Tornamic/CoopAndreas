#pragma once

#define OBJECT_PERIODIC_UPDATE_INTERVAL (1000)

class CNetworkObjectManager
{
public:
	static inline std::vector<CNetworkObject*> m_pObjects;

	static inline uint32_t ms_nNextObjectId = 1;

	static CNetworkObject* GetObject(objectid_t objectid);
	static CNetworkObject* GetObject(CEntity* object);
	static void Add(CNetworkObject* object);
	static void Remove(CNetworkObject* object);
	static objectid_t AllocateObjectId();
	static void ResetIdAllocator();
	static void RemoveHostedUnused();
	static void UpdateMovement();
};
