#include "stdafx.h"
#include "CNetworkVehicle.h"
#include <CKeySync.h>
#include "CNetworkVehicleManager.h"

CNetworkVehicle* CNetworkVehicleManager::GetVehicle(vehicleid_t vehicleid)
{
	if (!vehicleid.valid())
		return nullptr;

	for (int i = 0; i != m_pVehicles.size(); i++)
	{
		if (m_pVehicles[i]->m_vehicleId == vehicleid)
		{
			return m_pVehicles[i];
		}
	}
	return nullptr;
}
CNetworkVehicle* CNetworkVehicleManager::GetVehicle(CEntity* vehicle)
{
	for (int i = 0; i != m_pVehicles.size(); i++)
	{
		if (m_pVehicles[i]->m_pVehicle == vehicle)
		{
			return m_pVehicles[i];
		}
	}

	return nullptr;
}

void CNetworkVehicleManager::Add(CNetworkVehicle* vehicle)
{
	CNetworkVehicleManager::m_pVehicles.push_back(vehicle);
}

void CNetworkVehicleManager::Remove(CNetworkVehicle* vehicle)
{
	auto it = std::find(m_pVehicles.begin(), m_pVehicles.end(), vehicle);
	if (it != m_pVehicles.end())
	{
		m_pVehicles.erase(it);
	}
}

void CNetworkVehicleManager::UpdateDriver(CVehicle* pVehicle)
{
	if (auto pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pVehicle))
	{
		Packets::Vehicles::VehicleDriverUpdate vehicleDriverUpdate{};
		CPlayerPed* pPlayerPed = FindPlayerPed(0);

		vehicleDriverUpdate.vehicleid = pNetworkVehicle->m_vehicleId;
		vehicleDriverUpdate.vehicleSubType = static_cast<eVehicleType>(pVehicle->m_nVehicleSubType);
		vehicleDriverUpdate.pos = pVehicle->m_matrix->pos;
		vehicleDriverUpdate.roll = pVehicle->m_matrix->right;
		vehicleDriverUpdate.rot = pVehicle->m_matrix->up;
		vehicleDriverUpdate.velocity = pVehicle->m_vecMoveSpeed;
		vehicleDriverUpdate.turnSpeed = pVehicle->m_vecTurnSpeed;

		CWeapon& weapon = pPlayerPed->GetWeapon();
		vehicleDriverUpdate.playerWeapon.iWeaponType = weapon.m_eWeaponType;
		vehicleDriverUpdate.playerWeapon.iWeaponState = weapon.m_nState;
		vehicleDriverUpdate.playerWeapon.nAmmo = weapon.m_nAmmoInClip;

		vehicleDriverUpdate.playerHealth.iHealth = static_cast<uint8_t>(std::clamp(pPlayerPed->m_fHealth, 0.0f, 255.0f));
		vehicleDriverUpdate.playerHealth.iArmour = static_cast<uint8_t>(std::clamp(pPlayerPed->m_fArmour, 0.0f, 255.0f));
		CKeySync::CollectState(vehicleDriverUpdate.playerKeys);

		vehicleDriverUpdate.color1 = pVehicle->m_nPrimaryColor;
		vehicleDriverUpdate.color2 = pVehicle->m_nSecondaryColor;

		vehicleDriverUpdate.health = pVehicle->m_fHealth;

		vehicleDriverUpdate.paintjob = pVehicle->GetRemapIndex();

		if (pVehicle->m_nVehicleType == VEHICLE_AUTOMOBILE)
		{
			CAutomobile* pAutomobile = (CAutomobile*)pVehicle;
			vehicleDriverUpdate.miscComponentAngle = pAutomobile->m_wMiscComponentAngle;
		}

		if (pVehicle->m_nVehicleType == VEHICLE_BIKE)
		{
			CBike* pBike = (CBike*)pVehicle;
			vehicleDriverUpdate.bikeLean = pBike->m_rideAnimData.m_fDesiredLeanAngle;
		}

		if (pVehicle->m_nVehicleSubType == VEHICLE_PLANE)
		{
			CPlane* pPlane = (CPlane*)pVehicle;
			vehicleDriverUpdate.planeGearState = pPlane->m_fLandingGearStatus;
		}

		vehicleDriverUpdate.locked = pVehicle->m_eDoorLock;

		vehicleDriverUpdate.trailerid = CNetworkVehicleManager::GetTrailerId(pVehicle);

		if (vehicleDriverUpdate.vehicleSubType == VEHICLE_TRAIN)
		{
			CNetworkVehicleManager::GetTrainSnapshot((CTrain*)pVehicle, vehicleDriverUpdate.train);
		}

		GetPacketFactory().Send(vehicleDriverUpdate);
	}
}

void CNetworkVehicleManager::UpdateIdle()
{
	CNetworkVehicleManager::RemoveHostedUnused();

	for (auto pNetworkVehicle : m_pVehicles)
	{
		CVehicle* pVehicle = pNetworkVehicle->m_pVehicle;
		if (pVehicle == nullptr)
			continue;

		// a train engine driven by a ped needs the train state too, PedDriverUpdate does not have it
		bool bTrain = pVehicle->m_nVehicleType == VEHICLE_TRAIN;
		bool bPedDrivenTrain = bTrain && pNetworkVehicle->HasDriver() && !pVehicle->m_pDriver->IsPlayer();

		if (pNetworkVehicle->m_bSyncing && (!pNetworkVehicle->HasDriver() || bPedDrivenTrain))
		{
			Packets::Vehicles::VehicleIdleUpdate packet{};
			packet.vehicleid = pNetworkVehicle->m_vehicleId;
			packet.vehicleSubType = static_cast<eVehicleType>(pVehicle->m_nVehicleSubType);

			packet.pos = pVehicle->m_matrix->pos;
			packet.roll = pVehicle->m_matrix->right;
			packet.rot = pVehicle->m_matrix->up;
			packet.velocity = pVehicle->m_vecMoveSpeed;
			packet.turnSpeed = pVehicle->m_vecTurnSpeed;
			packet.color1 = pVehicle->m_nPrimaryColor;
			packet.color2 = pVehicle->m_nSecondaryColor;
			packet.health = pVehicle->m_fHealth;
			packet.paintjob = pVehicle->GetRemapIndex();

			if (CUtil::GetVehicleType(pVehicle) == eVehicleType::VEHICLE_PLANE)
			{
				CPlane* plane = (CPlane*)pVehicle;
				packet.planeGearState = plane->m_fLandingGearStatus;
			}

			packet.locked = pVehicle->m_eDoorLock;
			packet.trailerid = CNetworkVehicleManager::GetTrailerId(pVehicle);

			if (bTrain)
			{
				CNetworkVehicleManager::GetTrainSnapshot((CTrain*)pVehicle, packet.train);
			}
			GetPacketFactory().Send(packet);
		}
	}
}

void CNetworkVehicleManager::UpdatePassenger(CVehicle* pVehicle, CPlayerPed* pPlayerPed)
{
	 CNetworkVehicle* pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pVehicle);
	 if (pNetworkVehicle)
	 {
		 Packets::Vehicles::VehiclePassengerUpdate packet{};

		 CWeapon& weapon = pPlayerPed->GetWeapon();
		 packet.playerWeapon.iWeaponType = weapon.m_eWeaponType;
		 packet.playerWeapon.iWeaponState = weapon.m_nState;
		 packet.playerWeapon.nAmmo = weapon.m_nAmmoInClip;

		 packet.playerHealth.iHealth = static_cast<uint8_t>(std::clamp(pPlayerPed->m_fHealth, 0.0f, 255.0f));
		 packet.playerHealth.iArmour = static_cast<uint8_t>(std::clamp(pPlayerPed->m_fArmour, 0.0f, 255.0f));
		 CKeySync::CollectState(packet.playerKeys);

		 packet.vehicleid = pNetworkVehicle->m_vehicleId;
		 packet.driveby = CDriveBy::IsPedInDriveby(pPlayerPed);

		 for (int i = 0; i < pVehicle->m_nMaxPassengers; i++)
		 {
			 if (pVehicle->m_apPassengers[i] == pPlayerPed)
			 {
				 packet.seatid = i;
				 break;
			 }
		 }
		 GetPacketFactory().Send(packet);
	 }
}

vehicleid_t CNetworkVehicleManager::AllocateVehicleId()
{
	assert(CNetworkPlayerManager::m_localPlayerId.valid());
	return vehicleid_t(CNetworkPlayerManager::m_localPlayerId, ms_nNextVehicleId++);
}

void CNetworkVehicleManager::ResetIdAllocator()
{
	ms_nNextVehicleId = 1;
}

void CNetworkVehicleManager::RemoveHostedUnused()
{
	for (auto it = CNetworkVehicleManager::m_pVehicles.begin(); it != CNetworkVehicleManager::m_pVehicles.end();)
	{
		if ((*it)->m_bSyncing)
		{
			CVehicle* vehicle = (*it)->m_pVehicle;
			if (!IsVehiclePointerValid(vehicle))
			{
				delete* it;
				it = m_pVehicles.erase(it);
				continue;
			}
		}
		++it;
	}
}

void CNetworkVehicleManager::UpdateDamageSync()
{
	for (auto pNetworkVehicle : m_pVehicles)
	{
		if (pNetworkVehicle->m_bSyncing)
		{
			CVehicle* pVehicle = pNetworkVehicle->m_pVehicle;
			if (pVehicle && pVehicle->m_nVehicleType == VEHICLE_AUTOMOBILE)
			{
				CAutomobile* pAutomobile = (CAutomobile*)pVehicle;
				if (pNetworkVehicle->m_oldDamageState != pAutomobile->m_damageManager)
				{
					pNetworkVehicle->m_oldDamageState = pAutomobile->m_damageManager;
					Packets::Vehicles::VehicleDamage packet{};
					packet.vehicleid = pNetworkVehicle->m_vehicleId;
					packet.damageManager = pAutomobile->m_damageManager;
					GetPacketFactory().Send(packet);
				}
			}
		}
	}
}

vehicleid_t CNetworkVehicleManager::GetTrailerId(CVehicle* pVehicle)
{
	if (pVehicle->m_pTrailer)
	{
		if (auto pNetworkTrailer = CNetworkVehicleManager::GetVehicle(pVehicle->m_pTrailer))
			return pNetworkTrailer->m_vehicleId;
	}
	return vehicleid_t{};
}

void CNetworkVehicleManager::ApplyTrailer(CVehicle* pVehicle, vehicleid_t trailerid)
{
	CNetworkVehicle* pNetworkTrailer = CNetworkVehicleManager::GetVehicle(trailerid);
	CVehicle* pTrailer = pNetworkTrailer ? pNetworkTrailer->m_pVehicle : nullptr;
	if (pVehicle->m_pTrailer == pTrailer)
		return;

	if (pVehicle->m_pTrailer)
		pVehicle->m_pTrailer->BreakTowLink();

	if (pTrailer)
	{
		if (pTrailer->m_pTractor)
			pTrailer->BreakTowLink();

		pTrailer->SetTowLink(pVehicle, true);
	}
}

void CNetworkVehicleManager::GetTrainSnapshot(CTrain* pTrain, Packets::Vehicles::STrainSnapshot& snapshot)
{
	if (pTrain->m_pNextCarriage)
	{
		if (auto pNetworkCarriage = CNetworkVehicleManager::GetVehicle(pTrain->m_pNextCarriage))
			snapshot.nextCarriageid = pNetworkCarriage->m_vehicleId;
	}

	snapshot.trackId = pTrain->m_nTrackId;
	snapshot.bIsFrontCarriage = pTrain->m_nTrainFlags.bIsFrontCarriage;
	snapshot.bIsLastCarriage = pTrain->m_nTrainFlags.bIsLastCarriage;
	snapshot.bMissionTrain = pTrain->m_nTrainFlags.bMissionTrain;
	snapshot.bClockwiseDirection = pTrain->m_nTrainFlags.bClockwiseDirection;
	snapshot.bStopsAtStations = pTrain->m_nTrainFlags.bStopsAtStations;
	snapshot.bForceSlowDown = pTrain->m_nTrainFlags.bForceSlowDown;
	snapshot.bDerailed = pTrain->m_nTrainFlags.bNotOnARailRoad;
	snapshot.length = pTrain->m_fLength;
	snapshot.speed = pTrain->m_fTrainSpeed;
	snapshot.railDistance = pTrain->m_fCurrentRailDistance;
	snapshot.nodeIndex = pTrain->m_nNodeIndex;
	snapshot.cruiseSpeed = pTrain->m_autoPilot.m_nCruiseSpeed;
	snapshot.timeSinceStoppedAtStation = (uint16_t)std::min<unsigned int>(
		CTimer::m_snTimeInMilliseconds - pTrain->m_nTimeWhenStoppedAtStation, 40000);
}

void CNetworkVehicleManager::ApplyTrainSnapshot(CTrain* pTrain, const Packets::Vehicles::STrainSnapshot& snapshot)
{
	// link the carriage behind, the way CTrain::CreateMissionTrain does
	if (pTrain->m_pNextCarriage == nullptr)
	{
		CNetworkVehicle* pNetworkCarriage = CNetworkVehicleManager::GetVehicle(snapshot.nextCarriageid);
		CVehicle* pCarriage = pNetworkCarriage ? pNetworkCarriage->m_pVehicle : nullptr;
		if (pCarriage && pCarriage != pTrain && pCarriage->m_nVehicleType == VEHICLE_TRAIN &&
			((CTrain*)pCarriage)->m_pPrevCarriage == nullptr)
		{
			pTrain->m_pNextCarriage = (CTrain*)pCarriage;
			pCarriage->RegisterReference((CEntity**)&pTrain->m_pNextCarriage);
			((CTrain*)pCarriage)->m_pPrevCarriage = pTrain;
			pTrain->RegisterReference((CEntity**)&((CTrain*)pCarriage)->m_pPrevCarriage);
		}
	}

	pTrain->m_nTrackId = snapshot.trackId;
	pTrain->m_nTrainFlags.bIsFrontCarriage = snapshot.bIsFrontCarriage;
	pTrain->m_nTrainFlags.bIsLastCarriage = snapshot.bIsLastCarriage;
	pTrain->m_nTrainFlags.bMissionTrain = snapshot.bMissionTrain;
	pTrain->m_nTrainFlags.bClockwiseDirection = snapshot.bClockwiseDirection;
	pTrain->m_nTrainFlags.bStopsAtStations = snapshot.bStopsAtStations;
	pTrain->m_nTrainFlags.bForceSlowDown = snapshot.bForceSlowDown;
	pTrain->m_fLength = snapshot.length;
	pTrain->m_autoPilot.m_nCruiseSpeed = snapshot.cruiseSpeed;
	pTrain->m_nTimeWhenStoppedAtStation = CTimer::m_snTimeInMilliseconds - snapshot.timeSinceStoppedAtStation;

	// the carriages follow the one in front of them
	if (pTrain->m_pPrevCarriage == nullptr)
	{
		pTrain->m_fTrainSpeed = snapshot.speed;
		pTrain->m_fCurrentRailDistance = snapshot.railDistance;
		pTrain->m_nNodeIndex = snapshot.nodeIndex < NumTrackNodes[snapshot.trackId] ? snapshot.nodeIndex : 0;
	}

	if (snapshot.bDerailed && !pTrain->m_nTrainFlags.bNotOnARailRoad)
	{
		CNetworkVehicleManager::DerailTrain(CTrain::FindEngine(pTrain));
	}
}

// same as CTrain::ProcessControl does at 0x6F8DE5
void CNetworkVehicleManager::DerailTrain(CTrain* pTrain)
{
	for (; pTrain; pTrain = pTrain->m_pNextCarriage)
	{
		pTrain->m_nTrainFlags.bNotOnARailRoad = true;
		pTrain->m_nPhysicalFlags.bDisableCollisionForce = false;
		pTrain->m_nPhysicalFlags.b18 = false;
		pTrain->SetIsStatic(false);
	}
}

bool CNetworkVehicleManager::IsTrainOnRails(CVehicle* pVehicle)
{
	return pVehicle->m_nVehicleType == VEHICLE_TRAIN && !((CTrain*)pVehicle)->m_nTrainFlags.bNotOnARailRoad;
}
