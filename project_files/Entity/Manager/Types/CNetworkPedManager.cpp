#include "../../../CLocalPlayer.h"
#include "../../../CNetwork.h"
#include "../../../CPackets.h"
#include "../../Types/CNetworkVehicle.h"
#include "CNetworkPedManager.h"
#include "CNetworkVehicleManager.h"
#include <CTaskComplexWander.h>
#include <CVehicle.h>
#include "../../../CPacketHandler.h"
#include "../../../CNetwork.h"

void CNetworkPedManager::Update()
{
	if (!CLocalPlayer::m_bIsHost) return;

	for (CNetworkPed* networkPed : this->GetEntities())
	{
		CPed* ped = networkPed->m_pEntity;
		if (!ped) continue;

		CVehicle* vehicle = ped->m_pVehicle;
		CNetworkVehicle* networkVehicle = vehicle ? CNetworkVehicleManager::Instance().Get(vehicle) : nullptr;

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
	for (auto networkPed : this->GetEntities())
	{
		CPed* ped = networkPed->m_pEntity;

		if (ped == nullptr)
			continue;
		auto& syncData = networkPed->GetSyncData();
		ped->m_fAimingRotation = syncData.m_fAimingRotation;
		ped->m_fCurrentRotation = syncData.m_fCurrentRotation;
		ped->field_73C = syncData.m_fLookDirection;
	}
}

void CNetworkPedManager::AssignHost()
{
	for (auto networkPed : this->GetEntities())
	{
		CPed* ped = networkPed->m_pEntity;

		if (ped)
		{
			ped->SetCharCreatedBy(networkPed->m_nCreatedBy);

			CTaskComplexWander* task = plugin::CallAndReturn<CTaskComplexWander*, 0x673D00>(ped); // GetWanderTaskByPedType
			ped->m_pIntelligence->m_TaskMgr.SetTask(task, 0, false);
		}
	}
}