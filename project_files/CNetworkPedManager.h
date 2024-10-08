#pragma once
class CNetworkPedManager
{
public:
	static std::vector<CNetworkPed*> CNetworkPedManager::m_pPeds;
	static CNetworkPed* GetPed(int pedid);
	static CNetworkPed* GetPed(CPed* ped);
	static CNetworkPed* GetPed(CEntity* entity);
	static int GetFreeID();
	static void Add(CNetworkPed* ped);
	static void Remove(CNetworkPed* ped);
	static void Update();
	static void Process();
	static void AssignHost();
};

