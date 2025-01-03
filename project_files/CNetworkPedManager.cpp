#include "stdafx.h"
#include "CNetworkVehicle.h"
#include "CNetworkPed.h"

std::vector<CNetworkPed*> CNetworkPedManager::m_pPeds;
CNetworkPed* CNetworkPedManager::m_apTempPeds[255];

CNetworkPed* CNetworkPedManager::GetPed(int pedid)
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

CNetworkPed* CNetworkPedManager::GetPed(CPed* ped)
{
	if (ped == nullptr)
		return nullptr;

	for (int i = 0; i != m_pPeds.size(); i++)
	{
		if (m_pPeds[i]->m_pPed == ped)
		{
			return m_pPeds[i];
		}
	}

	return nullptr;
}

CNetworkPed* CNetworkPedManager::GetPed(CEntity* entity)
{
	if (entity == nullptr)
		return nullptr;

	for (int i = 0; i != m_pPeds.size(); i++)
	{
		if (m_pPeds[i]->m_pPed == entity)
		{
			return m_pPeds[i];
		}
	}

	return nullptr;
}

void CNetworkPedManager::Add(CNetworkPed* ped)
{
	CNetworkPedManager::m_pPeds.push_back(ped);
}

void CNetworkPedManager::Remove(CNetworkPed* ped)
{
	auto it = std::find(m_pPeds.begin(), m_pPeds.end(), ped);
	if (it != m_pPeds.end())
	{
		m_pPeds.erase(it);
	}
}

void CNetworkPedManager::Update()
{
	CNetworkPedManager::RemoveHostedUnused();

	for (CNetworkPed* networkPed : m_pPeds)
	{
		if (!networkPed->m_bSyncing)
			continue;

		CPed* ped = networkPed->m_pPed;
		if (!ped) continue;

		CVehicle* vehicle = ped->m_pVehicle;
		CNetworkVehicle* networkVehicle = vehicle ? CNetworkVehicleManager::GetVehicle(vehicle) : nullptr;

		if (networkVehicle && ped->m_nPedFlags.bInVehicle)
		{
			bool isDriver = (vehicle->m_pDriver == ped);

			if (isDriver)
			{
				auto pedDriverUpdatePacket = CPacketHandler::PedDriverUpdate__Collect(networkVehicle, networkPed);
				CNetwork::SendPacket(CPacketsID::PED_DRIVER_UPDATE, pedDriverUpdatePacket, sizeof(*pedDriverUpdatePacket), ENET_PACKET_FLAG_UNSEQUENCED);
				delete pedDriverUpdatePacket;
			}
			else
			{
				CPacketHandler::PedPassengerSync__Trigger(networkPed, networkVehicle);
			}
		}
		else
		{
			auto pedOnFootPacket = CPacketHandler::PedOnFoot__Collect(networkPed);
			CNetwork::SendPacket(CPacketsID::PED_ONFOOT, pedOnFootPacket, sizeof(*pedOnFootPacket), ENET_PACKET_FLAG_UNSEQUENCED);
			delete pedOnFootPacket;
		}
	}
}

void CNetworkPedManager::Process()
{
	for (auto networkPed : m_pPeds)
	{
		if (networkPed->m_bSyncing)
			continue;

		CPed* ped = networkPed->m_pPed;

		if (ped == nullptr)
			continue;

		ped->m_fAimingRotation = networkPed->m_fAimingRotation;
		ped->m_fCurrentRotation = networkPed->m_fCurrentRotation;
		ped->field_73C = networkPed->m_fLookDirection;
	}
}

void CNetworkPedManager::AssignHost()
{
	/*for (auto networkPed : m_pPeds)
	{
		CPed* ped = networkPed->m_pPed;

		if (ped)
		{
			ped->SetCharCreatedBy(networkPed->m_nCreatedBy);

			CTaskComplexWander* task = plugin::CallAndReturn<CTaskComplexWander*, 0x673D00>(ped); // GetWanderTaskByPedType
			ped->m_pIntelligence->m_TaskMgr.SetTask(task, 0, false);
		}
	}*/
}

unsigned char CNetworkPedManager::AddToTempList(CNetworkPed* networkPed)
{
	for (unsigned char i = 0; i < 255; i++)
	{
		if (m_apTempPeds[i] == nullptr)
		{
			m_apTempPeds[i] = networkPed;
			return i;
		}
	}

	return 255;
}

void CNetworkPedManager::RemoveHostedUnused()
{
	for (auto it = CNetworkPedManager::m_pPeds.begin(); it != CNetworkPedManager::m_pPeds.end();)
	{
		if ((*it)->m_bSyncing)
		{
			CPed* ped = (*it)->m_pPed;
			if (!IsPedPointerValid(ped))
			{
				delete* it;
				it = m_pPeds.erase(it);
				continue;
			}
		}
		++it;
	}
}