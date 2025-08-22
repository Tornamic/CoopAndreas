#include "stdafx.h"
#include "ProjectileHooks.h"
#include <CProjectileInfo.h>

bool __fastcall CWeapon__FireProjectile_Hook(CWeapon* This, int, CEntity* firingEntity, CVector* origin, CEntity* targetEntity, CVector* targetPos, float proj_force_)
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

bool __cdecl CWeapon__FireProjectile_AddProjectile_Hook(CEntity* creator, eWeaponType projectileType, CVector origin, float force, CVector* dir, CEntity* target)
{
	CPackets::AddProjectile packet{};
	packet.creator.SetEntity(creator);
	packet.projectileType = projectileType;
	packet.origin = origin;
	packet.force = force;
	packet.dir = CVector(0.0f, 0.0f, 0.0f);

	if (dir)
	{
		CChat::AddMessage("AddProjectile__Handle %p %d {%f %f %f} %f {%f %f %f} %p", creator, projectileType, origin.x, origin.y, origin.z, force, dir->x, dir->y, dir->z, target);
		packet.dir = *dir;
	}
	else
	{
		CChat::AddMessage("AddProjectile__Handle %p %d {%f %f %f} %f {from packet %f %f %f} %p", creator, projectileType, origin.x, origin.y, origin.z, force, packet.dir.x, packet.dir.y, packet.dir.z, target);
	}
	 
	packet.target.SetEntity(target);

	CNetwork::SendPacket(CPacketsID::ADD_PROJECTILE, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);

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
}
