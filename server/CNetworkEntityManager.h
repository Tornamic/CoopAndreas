#pragma once
#include "stdafx.h"

template <typename T>
class CNetworkEntityManager
{
private:
	std::vector<T*> m_pNetworkEntities;

protected:
	CNetworkEntityManager() {}

public:
	std::vector<T*>& GetEntities() { return this->m_pNetworkEntities; }

	void Add(T* networkEntity) { this->GetEntities().push_back(networkEntity); }

	void Remove(T* networkEntity)
	{
		auto& entities = this->GetEntities();
		entities.erase(std::remove(entities.begin(), entities.end(), networkEntity), entities.end());
	}

	T* Get(uint16_t networkId)
	{
		auto& entities = this->GetEntities();

		for (int i = 0; i != entities.size(); i++)
		{
			if (entities[i]->GetId() == networkId)
			{
				return entities[i];
			}
		}
		return nullptr;
	}

	uint16_t GetFreeId()
	{
		for (int i = 0; i != this->GetMaxCount(); i++)
		{
			if (this->Get(i) == nullptr)
				return i;
		}
		return -1;
	}

	virtual uint16_t GetMaxCount() const = 0;
};

