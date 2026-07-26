#include "stdafx.h"
#include "CNetworkVehicle.h"
#include <CKeySync.h>
#include "CNetworkVehicleManager.h"

CNetworkVehicle* CNetworkVehicleManager::GetVehicle(int vehicleid)
{
	for (int i = 0; i != m_pVehicles.size(); i++)
	{
		if (m_pVehicles[i]->m_nVehicleId == vehicleid)
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

		vehicleDriverUpdate.vehicleid = pNetworkVehicle->m_nVehicleId;
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

		if (pNetworkVehicle->m_bSyncing && !pNetworkVehicle->HasDriver())
		{
			Packets::Vehicles::VehicleIdleUpdate packet{};
			packet.vehicleid = pNetworkVehicle->m_nVehicleId;

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

		 packet.vehicleid = pNetworkVehicle->m_nVehicleId;
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

uint8_t CNetworkVehicleManager::AddToTempList(CNetworkVehicle* networkVehicle)
{
	for (uint8_t i = 0; i < ARRAY_SIZE(m_apTempVehicles); i++)
	{
		if (m_apTempVehicles[i] == nullptr)
		{
			m_apTempVehicles[i] = networkVehicle;
			return i;
		}
	}

	return 255;
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
					packet.vehicleid = pNetworkVehicle->m_nVehicleId;
					packet.damageManager = pAutomobile->m_damageManager;
					GetPacketFactory().Send(packet);
				}
			}
		}
	}
}