
#include "../core/CPedManager.h"

std::vector<CPed*> CPedManager::m_pPeds;

void CPedManager::Add(CPed* ped)
{
	m_pPeds.push_back(ped);
}

void CPedManager::Remove(CPed* ped)
{
	auto it = std::find(m_pPeds.begin(), m_pPeds.end(), ped);
	//std::find()
	if (it != m_pPeds.end())
	{
		m_pPeds.erase(it);
	}
}

CPed* CPedManager::GetPed(int pedid)
{
	for (int i = 0; i != m_pPeds.size(); i++)
	{
		if (m_pPeds[i]->m_nPedId == pedid)
		{
			return m_pPeds[i];
		}
	}
	return nullptr;
}