#include "stdafx.h"
#include "ProjectileHooks.h"
#include <CProjectileInfo.h>

bool __fastcall CWeapon__FireProjectile_Hook(CWeapon* This, SKIP_EDX, CEntity* firingEntity, CVector* origin, CEntity* targetEntity, CVector* targetPos, float proj_force_)
{
	if (firingEntity == FindPlayerPed(0))
	{
		return This->FireProjectile(firingEntity, origin, targetEntity, targetPos, proj_force_);
	}

	if (firingEntity->m_nType == ENTITY_TYPE_PED)
	{
		if (auto networkPed = CNetworkPedManager::GetPed(firingEntity))
		{
			if (networkPed->m_bSyncing)
			{
				return This->FireProjectile(firingEntity, origin, targetEntity, targetPos, proj_force_);
			}
		}
	}
	else if (firingEntity->m_nType == ENTITY_TYPE_VEHICLE)
	{
		if (auto networkVehicle = CNetworkVehicleManager::GetVehicle(firingEntity))
		{
			if (networkVehicle->m_bSyncing)
			{
				return This->FireProjectile(firingEntity, origin, targetEntity, targetPos, proj_force_);
			}
		}
	}

	return false;
}

// also used for CVehicle__FireHeatSeakingMissile
bool __cdecl CWeapon__FireProjectile_AddProjectile_Hook(CEntity* creator, eWeaponType projectileType, CVector origin, float force, CVector* dir, CEntity* target)
{
	if (!CLocalPlayer::GetIsHostingEntity(creator))
	{
		return false;
	}

	Packets::Players::AddProjectile packet{};
	packet.creator.SetEntity(creator);
	packet.projectileType = projectileType;
	packet.origin = origin;
	packet.force = force;

	if (dir)
	{
		packet.bDir = true;
		packet.dir = *dir;
	}
	else
	{
		packet.bDir = false;
		packet.dir = CVector(0.0f, 0.0f, 0.0f);
	}
	if (target)
	{
		packet.bTarget = true;
		packet.target.SetEntity(target);
	}
	else
	{
		packet.bTarget = false;
	}
	GetPacketFactory().Send(packet);

	return CProjectileInfo::AddProjectile(creator, projectileType, origin, force, dir, target);
}

void ProjectileHooks::InjectHooks()
{
	patch::RedirectCall(0x6D5261, CWeapon__FireProjectile_Hook);
	patch::RedirectCall(0x6D536D, CWeapon__FireProjectile_Hook);
	patch::RedirectCall(0x74265B, CWeapon__FireProjectile_Hook);
	patch::RedirectCall(0x742705, CWeapon__FireProjectile_Hook);
	patch::RedirectCall(0x742739, CWeapon__FireProjectile_Hook);
	patch::RedirectCall(0x74274E, CWeapon__FireProjectile_Hook);
	patch::RedirectCall(0x741A53, CWeapon__FireProjectile_AddProjectile_Hook);

	patch::RedirectCall(0x6E0749, CWeapon__FireProjectile_AddProjectile_Hook);
}
