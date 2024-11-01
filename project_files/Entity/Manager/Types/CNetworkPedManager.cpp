#include "stdafx.h"
#include "CNetworkVehicle.h"
#include "CNetworkPed.h"

std::vector<CNetworkPed*> CNetworkPedManager::m_pPeds;

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

int CNetworkPedManager::GetFreeID()
{
	if (!CLocalPlayer::m_bIsHost)
		return -1;

	for (int i = 0; i != MAX_SERVER_PEDS; i++)
	{
		if (CNetworkPedManager::GetPed(i) == nullptr)
			return i;
	}

	return -1;
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
	if (!CLocalPlayer::m_bIsHost) return;

	for (CNetworkPed* networkPed : m_pPeds)
	{
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
				CNetwork::SendPacket(ePacketType::PED_DRIVER_UPDATE, pedDriverUpdatePacket, sizeof(*pedDriverUpdatePacket), ENET_PACKET_FLAG_UNSEQUENCED);
				delete pedDriverUpdatePacket;
			}
			else
			{
				// todo: implement passenger update
			}
		}
		else
		{
			auto pedOnFootPacket = CPacketHandler::PedOnFoot__Collect(networkPed);
			CNetwork::SendPacket(ePacketType::PED_ONFOOT, pedOnFootPacket, sizeof(*pedOnFootPacket), ENET_PACKET_FLAG_UNSEQUENCED);
			delete pedOnFootPacket;
		}
	}
}

void CNetworkPedManager::Process()
{
	for (auto networkPed : m_pPeds)
	{
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
	for (auto networkPed : m_pPeds)
	{
		CPed* ped = networkPed->m_pPed;

		if (ped)
		{
			ped->SetCharCreatedBy(networkPed->m_nCreatedBy);

			CTaskComplexWander* task = plugin::CallAndReturn<CTaskComplexWander*, 0x673D00>(ped); // GetWanderTaskByPedType
			ped->m_pIntelligence->m_TaskMgr.SetTask(task, 0, false);
		}
	}
}