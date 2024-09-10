#pragma once
class CPedManager
{
public:
	static std::vector<CPed*> m_pPeds;
	static void Add(CPed* ped);
	static void Remove(CPed* ped);
	static CPed* GetPed(int pedid);
};

