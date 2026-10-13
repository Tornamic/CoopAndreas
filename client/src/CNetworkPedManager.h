#pragma once
class CNetworkPedManager
{
public:
	static std::vector<CNetworkPed*> m_pPeds;

	static inline uint32_t ms_nNextPedId = 1;

	static CNetworkPed* GetPed(pedid_t pedid);
	static CNetworkPed* GetPed(CEntity* entity);
	static void Add(CNetworkPed* ped);
	static void Remove(CNetworkPed* ped);
	static void Update();
	static void Process();
	static void AssignHost();
	static pedid_t AllocatePedId();
	static void ResetIdAllocator();
	static void RemoveHostedUnused();
};

