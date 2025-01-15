#include "stdafx.h"
#include "CTaskSync.h"
#include "CNetworkVehicle.h"
#include "CNetworkPed.h"
#include <game_sa/CTaskComplexEnterCarAsPassengerTimed.h>
// PlayerConnected

void CPacketHandler::PlayerConnected__Handle(void* data, int size)
{
	// get packet struct
	CPackets::PlayerConnected* packet = (CPackets::PlayerConnected*)data;

	// create new player
	CNetworkPlayer* player = new CNetworkPlayer(packet->id, CVector(2246.506f, -1259.552f, 23.9531f));

	// add player to list
	CNetworkPlayerManager::Add(player);

	player->m_bHasBeenConnectedBeforeMe = packet->isAlreadyConnected;

	if (!player->m_bHasBeenConnectedBeforeMe)
	{
		CChat::AddMessage("[Player] " + std::string(player->GetName()) + " connected");
	}
}

// PlayerDisconnected

void CPacketHandler::PlayerDisconnected__Handle(void* data, int size)
{
	// get packet struct
	CPackets::PlayerDisconnected* packet = (CPackets::PlayerDisconnected*)data;

	// get player instance
	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->id);

	if (player == nullptr)
		return;

	CChat::AddMessage("[Player] " + std::string(player->GetName()) + " disconnected");

	// remove from list
	CNetworkPlayerManager::Remove(player);

	// destroy player
	delete player;
}

// PlayerOnFoot

CPackets::PlayerOnFoot* CPacketHandler::PlayerOnFoot__Collect()
{
	// find local player
	CPlayerPed* player = FindPlayerPed(-1);

	// if player not created
	if (player == nullptr)
		return nullptr;
	
	// create PlayerOnFoot packet instance
	CPackets::PlayerOnFoot* packet = new CPackets::PlayerOnFoot;

	// get player position
	packet->position = player->m_matrix->pos;

	// get player move speed (velocity)
	packet->velocity = player->m_vecMoveSpeed;

	// get player facing angle
	packet->rotation = player->m_fCurrentRotation;
	
	// get player health, armour
	packet->health = (unsigned char)player->m_fHealth;
	packet->armour = (unsigned char)player->m_fArmour;

	// get player weapon in hands
	packet->weapon = player->m_aWeapons[player->m_nActiveWeaponSlot].m_eWeaponType;

	// get ammo in clip count
	packet->ammo = player->m_aWeapons[player->m_nActiveWeaponSlot].m_nAmmoInClip;

	// get crouch state
	packet->ducking = CUtil::IsDucked(player);

	packet->hasJetpack = CUtil::IsPedHasJetpack(player);

	packet->fightingStyle = player->m_nFightingStyle;

	return packet;
}

void CPacketHandler::PlayerOnFoot__Handle(void* data, int size)
{
	// get packet struct
	CPackets::PlayerOnFoot* packet = (CPackets::PlayerOnFoot*)data;

	// get player instance 
	CNetworkPlayer* networkPlayer = CNetworkPlayerManager::GetPlayer(packet->id);

	// check if player not connected
	if (networkPlayer == nullptr)
		return;

	CPlayerPed* player = networkPlayer->m_pPed;

	if (!player)
	{
		networkPlayer->CreatePed(networkPlayer->m_iPlayerId, packet->position);
		player = networkPlayer->m_pPed;
	}

	if (player->m_pVehicle && player->m_nPedFlags.bInVehicle)
	{
		networkPlayer->RemoveFromVehicle(player->m_pVehicle);
	}

	/*if (CUtil::IsPositionUpdateNeeded(networkPlayer->m_pPed->m_matrix->pos, packet->position))
	{*/
	networkPlayer->m_pPed->m_matrix->pos = packet->position;
	//}

	CUtil::GiveWeaponByPacket(networkPlayer, packet->weapon, packet->ammo);

	networkPlayer->UpdateHeading(packet->rotation);

	CUtil::SetPlayerJetpack(networkPlayer, packet->hasJetpack);

	if (CUtil::IsDucked(networkPlayer->m_pPed) != packet->ducking)
	{
		auto task = CTaskSimpleDuckToggle(packet->ducking);
		task.ProcessPed(networkPlayer->m_pPed);
	}

	networkPlayer->m_pPed->m_nFightingStyle = packet->fightingStyle;
	networkPlayer->m_pPed->m_nAllowedAttackMoves |= 15u;

	// save last onfoot sync
	networkPlayer->m_lOnFoot = packet;
}

// PlayerBulletShot

void CPacketHandler::PlayerBulletShot__Handle(void* data, int size)
{
	CPackets::PlayerBulletShot* packet = (CPackets::PlayerBulletShot*)data;

	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->playerid);

	CEntity* victim = nullptr;
	
	if (packet->targetid != -1)
	{
		switch ((eNetworkEntityType)packet->entityType)
		{
		case NETWORK_ENTITY_TYPE_PLAYER:
		{
			auto target = CNetworkPlayerManager::GetPlayer(packet->targetid);
			if (target && target->m_pPed)
			{
				victim = target->m_pPed;
			}
			break;
		}
		case NETWORK_ENTITY_TYPE_VEHICLE:
		{
			auto target = CNetworkVehicleManager::GetVehicle(packet->targetid);
			if (target && target->m_pVehicle)
			{
				victim = target->m_pVehicle;
			}
			break;
		}
		case NETWORK_ENTITY_TYPE_PED:
		{
			auto target = CNetworkPedManager::GetPed(packet->targetid);
			if (target && target->m_pPed)
			{
				victim = target->m_pPed;
			}
			break;
		}
		}

		if (packet->targetid == CNetworkPlayerManager::m_nMyId && packet->entityType == eNetworkEntityType::NETWORK_ENTITY_TYPE_PLAYER)
			victim = FindPlayerPed(0);
	}

	// change the ped type to prevent stats changing
	player->m_pPed->m_nPedType = PED_TYPE_CIVMALE;
	player->m_pPed->m_aWeapons[player->m_pPed->m_nActiveWeaponSlot].DoBulletImpact(player->m_pPed, victim, &packet->startPos, &packet->endPos, &packet->colPoint, packet->incrementalHit);
	player->m_pPed->m_nPedType = PED_TYPE_PLAYER1;
}

// PlayerHandshake

void CPacketHandler::PlayerHandshake__Handle(void* data, int size)
{
	CPackets::PlayerHandshake* packet = (CPackets::PlayerHandshake*)data;

	CNetworkPlayerManager::m_nMyId = packet->yourid;

	CPackets::PlayerGetName getNamePacket = {0};
	strcpy(getNamePacket.name, CLocalPlayer::m_Name);

	CNetwork::SendPacket(CPacketsID::PLAYER_GET_NAME, &getNamePacket, sizeof getNamePacket, ENET_PACKET_FLAG_RELIABLE);
}

// PlayerPlaceWaypoint

void CPacketHandler::PlayerPlaceWaypoint__Handle(void* data, int size)
{
	CPackets::PlayerPlaceWaypoint* packet = (CPackets::PlayerPlaceWaypoint*)data;

	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->playerid);

	player->m_bWaypointPlaced = packet->place;
	player->m_vecWaypointPos = &packet->position;

#ifdef PACKET_DEBUG_MESSAGES
	CChat::AddMessage("WAYPOINT PLACE %d %.0f %.0f\n", packet->place, packet->position.x, packet->position.y);
#endif
}

// PlayerGetName

void CPacketHandler::PlayerGetName__Handle(void* data, int size)
{
	CPackets::PlayerGetName* packet = (CPackets::PlayerGetName*)data;
	
	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->playerid);

	strcpy_s(player->m_Name, packet->name);

	if (!player->m_bHasBeenConnectedBeforeMe)
	{
		CChat::AddMessage("[Player] player " + std::to_string(player->m_iPlayerId) + " now aka " + player->m_Name);
	}

	CPacketHandler::GameWeatherTime__Trigger();
}

// PlayerSetHost

void CPacketHandler::PlayerSetHost__Handle(void* data, int size)
{
	CPackets::PlayerSetHost* packet = (CPackets::PlayerSetHost*)data;

	if (packet->playerid == CNetworkPlayerManager::m_nMyId)
	{
		CLocalPlayer::m_bIsHost = true;

		CPatch::RevertTemporaryPatchesForHost();

		CNetworkPedManager::AssignHost();
		CChat::AddMessage("[Player] You are the host now");

		CPacketHandler::GameWeatherTime__Trigger();
		return;
	}
	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->playerid);

	CChat::AddMessage("[Player] " + std::string(player->GetName()) + " is now the host");

	CLocalPlayer::m_bIsHost = false;
}

// AddExplosion

void CPacketHandler::AddExplosion__Handle(void* data, int size)
{
	CPackets::AddExplosion* packet = (CPackets::AddExplosion*)data;
	CExplosion::AddExplosion(nullptr, nullptr, (eExplosionType)packet->type, packet->pos, packet->time, packet->usesSound, packet->cameraShake, packet->isVisible);
}

// VehicleSpawn

void CPacketHandler::VehicleSpawn__Handle(void* data, int size)
{
	CPackets::VehicleSpawn* packet = (CPackets::VehicleSpawn*)data;
	
#ifdef PACKET_DEBUG_MESSAGES
	CChat::AddMessage("VEHICLE SPAWN %d %d %.1f %.1f %.1f %.1f", packet->vehicleid, packet->modelid, packet->pos.x, packet->pos.y, packet->pos.z, packet->rot);
#endif

	CNetworkVehicle* vehicle = new CNetworkVehicle
	(
		packet->vehicleid,
		packet->modelid,
		packet->pos,
		packet->rot,
		packet->color1,
		packet->color2,
		packet->createdBy
	);
	CNetworkVehicleManager::Add(vehicle);
}

// VehicleRemove

void CPacketHandler::VehicleRemove__Handle(void* data, int size)
{
	CPackets::VehicleRemove* packet = (CPackets::VehicleRemove*)data;

#ifdef PACKET_DEBUG_MESSAGES
	char buffer[128];
	sprintf(buffer, "VEHICLE REMOVE %d", packet->vehicleid);
	CChat::AddMessage(buffer);
#endif
	CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(packet->vehicleid);

	CNetworkVehicleManager::Remove(vehicle);
	delete vehicle;
}

// VehicleIdleUpdate

CPackets::VehicleIdleUpdate* CPacketHandler::VehicleIdleUpdate__Collect(CNetworkVehicle* vehicle)
{
	CPackets::VehicleIdleUpdate* packet = new CPackets::VehicleIdleUpdate;
	packet->vehicleid = vehicle->m_nVehicleId;
	packet->pos = vehicle->m_pVehicle->m_matrix->pos;
	packet->roll = vehicle->m_pVehicle->m_matrix->right;
	packet->rot = vehicle->m_pVehicle->m_matrix->up;
	packet->velocity = vehicle->m_pVehicle->m_vecMoveSpeed;
	packet->turnSpeed = vehicle->m_pVehicle->m_vecTurnSpeed;
	packet->color1 = vehicle->m_pVehicle->m_nPrimaryColor;
	packet->color2 = vehicle->m_pVehicle->m_nSecondaryColor;
	packet->health = vehicle->m_pVehicle->m_fHealth;

	if (CUtil::GetVehicleType(vehicle->m_pVehicle) == eVehicleType::VEHICLE_PLANE)
	{
		CPlane* plane = (CPlane*)vehicle->m_pVehicle;
		packet->planeGearState = plane->m_fLandingGearStatus > 0.0f ? 1.0f : 0.0f;
	}

	packet->locked = vehicle->m_pVehicle->m_eDoorLock;

	return packet;
}

void CPacketHandler::VehicleIdleUpdate__Handle(void* data, int size)
{
	CPackets::VehicleIdleUpdate* packet = (CPackets::VehicleIdleUpdate*)data;
	CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(packet->vehicleid);

	if (vehicle == nullptr)
		return;

	if (!vehicle->m_pVehicle)
		return;

	if (vehicle->m_pVehicle->m_matrix == nullptr)
		return;

	vehicle->m_pVehicle->m_matrix->pos = packet->pos;

	if(CUtil::IsPositionUpdateNeeded(vehicle->m_pVehicle->m_matrix->right, packet->roll, 1))
		vehicle->m_pVehicle->m_matrix->right = packet->roll;

	if (CUtil::IsPositionUpdateNeeded(vehicle->m_pVehicle->m_matrix->up, packet->rot, 1))
		vehicle->m_pVehicle->m_matrix->up = packet->rot;

	vehicle->m_pVehicle->m_vecMoveSpeed = packet->velocity;
	vehicle->m_pVehicle->m_vecTurnSpeed = packet->turnSpeed;

	vehicle->m_pVehicle->m_nPrimaryColor = packet->color1;
	vehicle->m_pVehicle->m_nSecondaryColor = packet->color2;
	vehicle->m_pVehicle->m_fHealth = packet->health;

	if (CUtil::GetVehicleType(vehicle->m_pVehicle) == eVehicleType::VEHICLE_PLANE)
	{
		CPlane* plane = (CPlane*)vehicle->m_pVehicle;

		plane->m_fLandingGearStatus = packet->planeGearState;
	}

	vehicle->m_pVehicle->m_eDoorLock = (eDoorLock)packet->locked;
}

// VehicleDriverUpdate

CPackets::VehicleDriverUpdate* CPacketHandler::VehicleDriverUpdate__Collect(CNetworkVehicle* vehicle)
{
	CPackets::VehicleDriverUpdate* packet = new CPackets::VehicleDriverUpdate;
	CPlayerPed* player = FindPlayerPed(0);

	// vehicle data
	packet->vehicleid =			vehicle->m_nVehicleId;
	packet->pos =				vehicle->m_pVehicle->m_matrix->pos;
	packet->roll =				vehicle->m_pVehicle->m_matrix->right;
	packet->rot =				vehicle->m_pVehicle->m_matrix->up;
	packet->velocity =			vehicle->m_pVehicle->m_vecMoveSpeed;

	// player data
	packet->ammo =				player->m_aWeapons[player->m_nActiveWeaponSlot].m_nAmmoInClip;
	packet->playerArmour =		(unsigned char)player->m_fArmour;
	packet->playerHealth =		(unsigned char)player->m_fHealth;
	packet->weapon =			player->m_aWeapons[player->m_nActiveWeaponSlot].m_eWeaponType;

	packet->color1 = vehicle->m_pVehicle->m_nPrimaryColor;
	packet->color2 = vehicle->m_pVehicle->m_nSecondaryColor;

	packet->health = vehicle->m_pVehicle->m_fHealth;

	packet->paintjob = vehicle->m_nPaintJob;

	if (vehicle->m_pVehicle->m_nModelIndex == 520)
	{
		CAutomobile* automobile = (CAutomobile*)vehicle->m_pVehicle;
		packet->miscComponentAngle = automobile->m_wMiscComponentAngle;
	}

	eVehicleType vehicleType = CUtil::GetVehicleType(vehicle->m_pVehicle);
	if (vehicleType == eVehicleType::VEHICLE_BIKE || vehicleType == eVehicleType::VEHICLE_BMX)
	{
		CBike* bike = (CBike*)vehicle->m_pVehicle;

		packet->bikeLean = bike->m_rideAnimData.m_fAnimLean;
	}

	if (vehicleType == eVehicleType::VEHICLE_PLANE)
	{
		CPlane* plane = (CPlane*)vehicle->m_pVehicle;
		packet->planeGearState = plane->m_fLandingGearStatus;
	}

	packet->locked = vehicle->m_pVehicle->m_eDoorLock;

	return packet;
}

void CPacketHandler::VehicleDriverUpdate__Handle(void* data, int size)
{
	CPackets::VehicleDriverUpdate* packet = (CPackets::VehicleDriverUpdate*)data;

	CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(packet->vehicleid);
	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->playerid);

	if (vehicle == nullptr || player == nullptr)
		return;

	if (!CUtil::IsValidEntityPtr(vehicle->m_pVehicle))
		return;

	/*if (vehicle->m_pVehicle == nullptr)
	{
		vehicle->CreateVehicle(vehicle->m_nVehicleId, vehicle->m_nModelId, packet->pos, 0.f, packet->color1, packet->color2);
		return;
	}

	if (player->m_pPed == nullptr)
	{
		player->CreatePed(player->m_iPlayerId, packet->pos);
		return;
	}*/

	if (player->m_pPed->m_pVehicle != vehicle->m_pVehicle || !player->m_pPed->m_nPedFlags.bInVehicle)
	{
		player->WarpIntoVehicleDriver(vehicle->m_pVehicle);
	}

	vehicle->m_pVehicle->m_matrix->pos = packet->pos;
	vehicle->m_pVehicle->m_matrix->right = packet->roll;
	vehicle->m_pVehicle->m_matrix->up = packet->rot;
	vehicle->m_pVehicle->m_vecMoveSpeed = packet->velocity;
	
	CUtil::GiveWeaponByPacket(player, packet->weapon, packet->ammo);
	
	player->m_lOnFoot->armour = packet->playerArmour;
	player->m_lOnFoot->health = packet->playerHealth;

	vehicle->m_pVehicle->m_nPrimaryColor = packet->color1;
	vehicle->m_pVehicle->m_nSecondaryColor = packet->color2;

	vehicle->m_pVehicle->m_fHealth = packet->health;

	if(vehicle->m_nPaintJob != packet->paintjob)
		vehicle->m_pVehicle->SetRemap(packet->paintjob);

	if (vehicle->m_pVehicle->m_nModelIndex == 520)
	{
		CAutomobile* automobile = (CAutomobile*)vehicle->m_pVehicle;
		automobile->m_wMiscComponentAngle = packet->miscComponentAngle;
	}

	eVehicleType vehicleType = CUtil::GetVehicleType(vehicle->m_pVehicle);
	if (vehicleType == eVehicleType::VEHICLE_BIKE || vehicleType == eVehicleType::VEHICLE_BMX)
	{
		CBike* bike = (CBike*)vehicle->m_pVehicle;

		*(float*)((DWORD)&*bike + 0x64C) = packet->bikeLean;
	}

	if (vehicleType == eVehicleType::VEHICLE_PLANE)
	{
		CPlane* plane = (CPlane*)vehicle->m_pVehicle;

		plane->m_fLandingGearStatus = packet->planeGearState;
	}

	vehicle->m_pVehicle->m_eDoorLock = (eDoorLock)packet->locked;
}

// VehicleEnter

void CPacketHandler::VehicleEnter__Handle(void* data, int size)
{
	CPackets::VehicleEnter* packet = (CPackets::VehicleEnter*)data;
	
	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->playerid);
	CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(packet->vehicleid);

	if (player == nullptr || vehicle == nullptr || vehicle->m_pVehicle == nullptr)
	{
		return;
	}
	
	if (!CUtil::IsValidEntityPtr(vehicle->m_pVehicle) || !CUtil::IsValidEntityPtr(player->m_pPed))
		return;

#ifdef PACKET_DEBUG_MESSAGES
	CChat::AddMessage("player %d entered vehicleid %d %s", packet->playerid, packet->vehicleid, packet->seatid != 0 ? "as passenger" : "");
#endif

	if (packet->passenger == 0) // driver
	{
		if (packet->force)
		{
			for (unsigned char i = 0; i < 5; i++)
			{
				if (vehicle->m_pVehicle)
					break;

				Sleep(100);
			}

			player->m_pPed->m_nPedFlags.CantBeKnockedOffBike = 1; // 1 - never
			plugin::Command<Commands::WARP_CHAR_INTO_CAR>(CPools::GetPedRef(player->m_pPed), CPools::GetVehicleRef(vehicle->m_pVehicle));
		}
		else
		{
			player->m_pPed->m_nPedFlags.CantBeKnockedOffBike = 1; // 1 - never
			plugin::Command<Commands::TASK_ENTER_CAR_AS_DRIVER>(CPools::GetPedRef(player->m_pPed), CPools::GetVehicleRef(vehicle->m_pVehicle), 3000);
		}
	}
	else
	{
		player->EnterVehiclePassenger(vehicle->m_pVehicle, packet->seatid);
	}
}

// VehicleExit

void CPacketHandler::VehicleExit__Handle(void* data, int size)
{
	CPackets::VehicleExit* packet = (CPackets::VehicleExit*)data;

	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->playerid);

	if (player == nullptr)
	{
		return;
	}

	if (player->m_pPed->m_pVehicle == nullptr)
	{
		return;
	}

	if (!CUtil::IsValidEntityPtr(player->m_pPed->m_pVehicle) || !CUtil::IsValidEntityPtr(player->m_pPed))
		return;

#ifdef PACKET_DEBUG_MESSAGES
	CChat::AddMessage("player %d exited from vehicle", packet->playerid);
#endif
	if (packet->force)
	{
		CVector doorPos{};
		player->m_pPed->m_pVehicle->GetComponentWorldPosition(8, doorPos); // right front door (driver)
		player->m_pPed->m_nPedFlags.CantBeKnockedOffBike = 2; // 2 - normal
		plugin::Command<Commands::WARP_CHAR_FROM_CAR_TO_COORD>(CPools::GetPedRef(player->m_pPed), doorPos.x, doorPos.y, doorPos.z);
	}
	else
	{
		player->m_pPed->m_nPedFlags.CantBeKnockedOffBike = 2; // 2 - normal
		plugin::Command<Commands::TASK_LEAVE_CAR>(CPools::GetPedRef(player->m_pPed), CPools::GetVehicleRef(player->m_pPed->m_pVehicle));
	}
}

// VehicleDamage

void CPacketHandler::VehicleDamage__Handle(void* data, int size)
{
#ifdef PACKET_DEBUG_MESSAGES
	CChat::AddMessage("VehicleDamage__Handle");
#endif
	CPackets::VehicleDamage* packet = (CPackets::VehicleDamage*)data;

	if (auto networkVehicle = CNetworkVehicleManager::GetVehicle(packet->vehicleid))
	{
		if (auto vehicle = networkVehicle->m_pVehicle)
		{
			eVehicleType vehicleType = CUtil::GetVehicleType(vehicle);
			switch (vehicleType)
			{
			case VEHICLE_AUTOMOBILE:
			case VEHICLE_MTRUCK:
			case VEHICLE_QUAD:
			case VEHICLE_HELI:
			case VEHICLE_PLANE:
			case VEHICLE_TRAILER:
			((CAutomobile*)vehicle)->m_damageManager = packet->damageManager;
			vehicle->SetupDamageAfterLoad();
				break;
			}
		}
	}

}

// VehicleComponentAdd

void CPacketHandler::VehicleComponentAdd__Handle(void* data, int size)
{
	CPackets::VehicleComponentAdd* packet = (CPackets::VehicleComponentAdd*)data;

	CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(packet->vehicleid);

	if (vehicle == nullptr)
		return;

	CStreaming::RequestModel(packet->componentid, eStreamingFlags::GAME_REQUIRED);
	CStreaming::LoadAllRequestedModels(false);
	CStreaming::RequestVehicleUpgrade(packet->componentid, eStreamingFlags::GAME_REQUIRED);

	char count = 10;
	while (!CStreaming::HasVehicleUpgradeLoaded(packet->componentid) && count) 
	{
		Sleep(5);
		count--;
	}

	if (!count)
		return;

	vehicle->m_pVehicle->AddVehicleUpgrade(packet->componentid);
}

// VehicleComponentRemove

void CPacketHandler::VehicleComponentRemove__Handle(void* data, int size)
{
	CPackets::VehicleComponentAdd* packet = (CPackets::VehicleComponentAdd*)data;

	CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(packet->vehicleid);

	if (vehicle == nullptr)
		return;

	vehicle->m_pVehicle->RemoveVehicleUpgrade(packet->componentid);
}

// VehiclePassengerUpdate

CPackets::VehiclePassengerUpdate* CPacketHandler::VehiclePassengerUpdate__Collect(CNetworkVehicle* vehicle, CPlayerPed* localPlayer)
{
	CPackets::VehiclePassengerUpdate* packet = new CPackets::VehiclePassengerUpdate;

	// player data
	packet->ammo = localPlayer->m_aWeapons[localPlayer->m_nActiveWeaponSlot].m_nAmmoInClip;
	packet->playerArmour = (unsigned char)localPlayer->m_fArmour;
	packet->playerHealth = (unsigned char)localPlayer->m_fHealth;
	packet->weapon = localPlayer->m_aWeapons[localPlayer->m_nActiveWeaponSlot].m_eWeaponType;
	packet->vehicleid = vehicle->m_nVehicleId;
	packet->driveby = CDriveBy::IsPedInDriveby(localPlayer);

	for (int i = 0; i < vehicle->m_pVehicle->m_nMaxPassengers; i++)
	{
		if (vehicle->m_pVehicle->m_apPassengers[i] == localPlayer)
		{
			packet->seatid = i;
			break;
		}
	}

	return packet;
}

void CPacketHandler::VehiclePassengerUpdate__Handle(void* data, int size)
{
	CPackets::VehiclePassengerUpdate* packet = (CPackets::VehiclePassengerUpdate*)data;

	CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(packet->vehicleid);
	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->playerid);

	if (vehicle == nullptr || player == nullptr)
		return;

	if (vehicle->m_pVehicle == nullptr)
		return;

	if (!CUtil::IsValidEntityPtr(vehicle->m_pVehicle) || !CUtil::IsValidEntityPtr(player->m_pPed))
		return;

	if (player->m_pPed == nullptr)
		return;

	if (!player->m_pPed->m_nPedFlags.bInVehicle || player->m_pPed->m_pVehicle != vehicle->m_pVehicle || vehicle->m_pVehicle->m_apPassengers[packet->seatid] != player->m_pPed)
	{
#ifdef PACKET_DEBUG_MESSAGES
		CChat::AddMessage("forcing enter passenger %d", player->m_iPlayerId);
#endif

		player->WarpIntoVehiclePassenger(vehicle->m_pVehicle, packet->seatid);
	}

	CUtil::GiveWeaponByPacket(player, packet->weapon, packet->ammo);

	player->m_pPed->m_fArmour = player->m_lOnFoot->armour = packet->playerArmour;
	player->m_pPed->m_fHealth = player->m_lOnFoot->health = packet->playerHealth;

	if (packet->driveby && !CDriveBy::IsPedInDriveby(player->m_pPed))
	{
		CDriveBy::StartDriveby(player->m_pPed);
	}
	else if (!packet->driveby && CDriveBy::IsPedInDriveby(player->m_pPed))
	{
		CDriveBy::StopDriveby(player->m_pPed);
	}
}

// PlayerChatMessage

void CPacketHandler::PlayerChatMessage__Handle(void* data, int size)
{
	CPackets::PlayerChatMessage* packet = (CPackets::PlayerChatMessage*)data;
	
	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->playerid);

	if (player)
	{
		CChat::SendPlayerMessage(player->GetName(), player->m_iPlayerId, packet->message);
	}
}

// PedSpawn

void CPacketHandler::PedSpawn__Handle(void* data, int size)
{
	CPackets::PedSpawn* packet = (CPackets::PedSpawn*)data;

#ifdef PACKET_DEBUG_MESSAGES
	CChat::AddMessage("PED SPAWN %d %d %.1f %.1f %.1f %d %d", packet->pedid, packet->modelId, packet->pos.x, packet->pos.y, packet->pos.z, packet->pedType, packet->createdBy);
#endif

	CNetworkPed* ped = new CNetworkPed(packet->pedid, (int)packet->modelId, (ePedType)packet->pedType, packet->pos, packet->createdBy);

	CNetworkPedManager::Add(ped);
}

// PedRemove

void CPacketHandler::PedRemove__Handle(void* data, int size)
{
	CPackets::PedRemove* packet = (CPackets::PedRemove*)data;

#ifdef PACKET_DEBUG_MESSAGES
	CChat::AddMessage("PED REMOVE %d", packet->pedid);
#endif

	CNetworkPed* ped = CNetworkPedManager::GetPed(packet->pedid);

	if (ped)
	{
		CNetworkPedManager::Remove(ped);
		delete ped;
	}
}

// PedOnFoot

CPackets::PedOnFoot* CPacketHandler::PedOnFoot__Collect(CNetworkPed* networkPed)
{
	CPackets::PedOnFoot* packet = new CPackets::PedOnFoot;
	CPed* ped = networkPed->m_pPed;

	packet->pedid = networkPed->m_nPedId;
	packet->pos = ped->m_matrix->pos;
	packet->velocity = ped->m_vecMoveSpeed;
	packet->health = (unsigned char)ped->m_fHealth;
	packet->armour = (unsigned char)ped->m_fArmour;
	packet->weapon = ped->m_aWeapons[ped->m_nActiveWeaponSlot].m_eWeaponType;
	packet->ammo = ped->m_aWeapons[ped->m_nActiveWeaponSlot].m_nAmmoInClip;
	packet->aimingRotation = ped->m_fAimingRotation;
	packet->currentRotation = ped->m_fCurrentRotation;
	packet->lookDirection = ped->field_73C; // look direction (rad)
	packet->moveState = (unsigned char)ped->m_nMoveState;
	packet->ducked = CUtil::IsDucked(ped);
	
	CTaskSimpleUseGun* useGun = ped->m_pIntelligence->GetTaskUseGun();

	if (useGun)
	{
		packet->aiming = true;
		packet->weaponAim = useGun->m_pTarget && (useGun->m_vecTarget.x == 0.f || useGun->m_vecTarget.y == 0.f) ? useGun->m_pTarget->GetPosition() : useGun->m_vecTarget;
	}

	packet->fightingStyle = ped->m_nFightingStyle;

	return packet;
}

void CPacketHandler::PedOnFoot__Handle(void* data, int size)
{
	CPackets::PedOnFoot* packet = (CPackets::PedOnFoot*)data;

	CNetworkPed* ped = CNetworkPedManager::GetPed(packet->pedid);

	if (!ped)
		return;

	if (!ped->m_pPed)
		return;

	if (ped->m_pPed->m_pVehicle && ped->m_pPed->m_nPedFlags.bInVehicle && CUtil::IsValidEntityPtr(ped->m_pPed->m_pVehicle))
	{
		//plugin::Command<Commands::TASK_LEAVE_CAR>(CPools::GetPedRef(ped->m_pPed), CPools::GetVehicleRef(ped->m_pPed->m_pVehicle));
		//plugin::Command<Commands::WARP_CHAR_FROM_CAR_TO_COORD>(CPools::GetPedRef(ped->m_pPed), packet->pos.x, packet->pos.y, packet->pos.z);
		ped->RemoveFromVehicle(ped->m_pPed->m_pVehicle);
	}

	CUtil::GiveWeaponByPacket(ped, packet->weapon, packet->ammo);

	ped->m_pPed->m_matrix->pos = packet->pos;
	ped->m_fCurrentRotation = ped->m_pPed->m_fCurrentRotation = packet->currentRotation;
	ped->m_fAimingRotation = ped->m_pPed->m_fAimingRotation = packet->aimingRotation;
	ped->m_fLookDirection = ped->m_pPed->field_73C = packet->lookDirection;
	ped->m_pPed->m_fHealth = packet->health;
	ped->m_fHealth = packet->health;
	ped->m_pPed->m_fArmour = packet->armour;
	ped->m_vecVelocity = packet->velocity;
	ped->m_nMoveState = (eMoveState)packet->moveState;

	if (CUtil::IsDucked(ped->m_pPed) != packet->ducked)
	{
		CTaskSimpleDuckToggle task = CTaskSimpleDuckToggle(packet->ducked);
		task.ProcessPed(ped->m_pPed);
	}

	if (packet->aiming)
	{
		plugin::Command<COMMAND_TASK_AIM_GUN_AT_COORD>(CPools::GetPedRef(ped->m_pPed), packet->weaponAim.x, packet->weaponAim.y, packet->weaponAim.z, 200);
	}
	else
	{
		if (auto useGun = ped->m_pPed->m_pIntelligence->GetTaskUseGun())
		{
			useGun->m_bIsFinished = true;
		}
	}

	ped->m_pPed->m_nFightingStyle = packet->fightingStyle;
}

// GameWeatherTime

CPackets::GameWeatherTime* CPacketHandler::GameWeatherTime__Collect()
{
	CPackets::GameWeatherTime* packet = new CPackets::GameWeatherTime;
	packet->newWeather = (unsigned char)CWeather::NewWeatherType;
	packet->oldWeather = (unsigned char)CWeather::OldWeatherType;
	packet->forcedWeather = (unsigned char)CWeather::ForcedWeatherType;
	packet->currentMonth = CClock::ms_nGameClockMonth;
	packet->currentDay = CClock::CurrentDay;
	packet->currentHour = CClock::ms_nGameClockHours;
	packet->currentMinute = CClock::ms_nGameClockMinutes;
	packet->gameTickCount = CClock::ms_nMillisecondsPerGameMinute;
	return packet;
}

void CPacketHandler::GameWeatherTime__Handle(void* data, int size)
{
	CPackets::GameWeatherTime* packet = (CPackets::GameWeatherTime*)data;

	CWeather::OldWeatherType = //packet->oldWeather;
	CWeather::NewWeatherType = packet->newWeather;
	//CWeather::ForcedWeatherType = packet->forcedWeather;
	CClock::ms_nGameClockMonth = packet->currentMonth;
	CClock::CurrentDay = packet->currentDay;
	CClock::ms_nGameClockHours = packet->currentHour;
	CClock::ms_nGameClockMinutes = packet->currentMinute;
	CClock::ms_nMillisecondsPerGameMinute = packet->gameTickCount;
}

void CPacketHandler::GameWeatherTime__Trigger()
{
	if (!CLocalPlayer::m_bIsHost)
		return;

	CPackets::GameWeatherTime* packet = CPacketHandler::GameWeatherTime__Collect();
	CNetwork::SendPacket(CPacketsID::GAME_WEATHER_TIME, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE);
}

// PlayerKeySync

void CPacketHandler::PlayerKeySync__Handle(void* data, int size)
{
	CPackets::PlayerKeySync* packet = (CPackets::PlayerKeySync*)data;

	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->playerid);
	player->m_compressedControllerState = packet->newState;
}

// PedAddTask

void CPacketHandler::PedAddTask__Handle(void* data, int size)
{
	//CTaskSync::DeSerializeTask(data);
}

// PedDriverUpdate

CPackets::PedDriverUpdate* CPacketHandler::PedDriverUpdate__Collect(CNetworkVehicle* vehicle, CNetworkPed* ped)
{
	CPackets::PedDriverUpdate* packet = new CPackets::PedDriverUpdate;
	
	// ped data
	packet->pedid = ped->m_nPedId;
	packet->ammo = ped->m_pPed->m_aWeapons[ped->m_pPed->m_nActiveWeaponSlot].m_nAmmoInClip;
	packet->pedArmour = (unsigned char)ped->m_pPed->m_fArmour;
	packet->pedHealth = (unsigned char)ped->m_pPed->m_fHealth;
	packet->weapon = ped->m_pPed->m_aWeapons[ped->m_pPed->m_nActiveWeaponSlot].m_eWeaponType;

	// vehicle data
	packet->vehicleid = vehicle->m_nVehicleId;
	packet->pos = vehicle->m_pVehicle->m_matrix->pos;
	packet->roll = vehicle->m_pVehicle->m_matrix->right;
	packet->rot = vehicle->m_pVehicle->m_matrix->up;
	packet->velocity = vehicle->m_pVehicle->m_vecMoveSpeed;
	packet->turnSpeed = vehicle->m_pVehicle->m_vecTurnSpeed;

	packet->color1 = vehicle->m_pVehicle->m_nPrimaryColor;
	packet->color2 = vehicle->m_pVehicle->m_nSecondaryColor;

	packet->health = vehicle->m_pVehicle->m_fHealth;

	packet->paintjob = vehicle->m_nPaintJob;

	eVehicleType vehicleType = CUtil::GetVehicleType(vehicle->m_pVehicle);
	if (vehicleType == eVehicleType::VEHICLE_BIKE || vehicleType == eVehicleType::VEHICLE_BMX)
	{
		CBike* bike = (CBike*)vehicle->m_pVehicle;

		packet->bikeLean = bike->m_rideAnimData.m_fAnimLean;
	}

	if (vehicleType == eVehicleType::VEHICLE_PLANE)
	{
		CPlane* plane = (CPlane*)vehicle->m_pVehicle;
		packet->planeGearState = plane->m_fLandingGearStatus;
	}

	packet->locked = vehicle->m_pVehicle->m_eDoorLock;

	return packet;
}

void CPacketHandler::PedDriverUpdate__Handle(void* data, int size)
{
	CPackets::PedDriverUpdate* packet = (CPackets::PedDriverUpdate*)data;

	CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(packet->vehicleid);
	CNetworkPed* ped = CNetworkPedManager::GetPed(packet->pedid);

	if (vehicle == nullptr || ped == nullptr || ped->m_pPed == nullptr || vehicle->m_pVehicle == nullptr || !CUtil::IsValidEntityPtr(vehicle->m_pVehicle) || !CUtil::IsValidEntityPtr(ped->m_pPed))
		return;

	if (ped->m_pPed->m_pVehicle != vehicle->m_pVehicle || !ped->m_pPed->m_nPedFlags.bInVehicle)
		ped->WarpIntoVehicleDriver(vehicle->m_pVehicle);

	if(CUtil::IsPositionUpdateNeeded(packet->pos, vehicle->m_pVehicle->m_matrix->pos))
		vehicle->m_pVehicle->m_matrix->pos = packet->pos;

	vehicle->m_pVehicle->m_matrix->right = packet->roll;
	vehicle->m_pVehicle->m_matrix->up = packet->rot;
	ped->m_vecVelocity = packet->velocity;
	vehicle->m_pVehicle->m_vecMoveSpeed = packet->velocity;
	vehicle->m_pVehicle->m_vecTurnSpeed = packet->turnSpeed;

	CUtil::GiveWeaponByPacket(ped, packet->weapon, packet->ammo);

	ped->m_pPed->m_fArmour = packet->pedArmour;
	ped->m_pPed->m_fHealth = packet->pedHealth;
	ped->m_fHealth = packet->pedHealth;

	vehicle->m_pVehicle->m_nPrimaryColor = packet->color1;
	vehicle->m_pVehicle->m_nSecondaryColor = packet->color2;

	vehicle->m_pVehicle->m_fHealth = packet->health;

	if (vehicle->m_nPaintJob != packet->paintjob)
		vehicle->m_pVehicle->SetRemap(packet->paintjob);

	eVehicleType vehicleType = CUtil::GetVehicleType(vehicle->m_pVehicle);
	if (vehicleType == eVehicleType::VEHICLE_BIKE || vehicleType == eVehicleType::VEHICLE_BMX)
	{
		CBike* bike = (CBike*)vehicle->m_pVehicle;

		*(float*)((DWORD)&*bike + 0x64C) = packet->bikeLean;
	}

	if (CUtil::GetVehicleType(vehicle->m_pVehicle) == eVehicleType::VEHICLE_PLANE)
	{
		CPlane* plane = (CPlane*)vehicle->m_pVehicle;

		plane->m_fLandingGearStatus = packet->planeGearState;
	}

	vehicle->m_pVehicle->m_eDoorLock = (eDoorLock)packet->locked;
}

// PedShotSync

void CPacketHandler::PedShotSync__Handle(void* data, int size)
{
	CPackets::PedShotSync* packet = (CPackets::PedShotSync*)data;
	
	CNetworkPed* ped = CNetworkPedManager::GetPed(packet->pedid);

	if (ped && ped->m_pPed)
	{
		ped->m_pPed->m_aWeapons[ped->m_pPed->m_nActiveWeaponSlot].Fire(ped->m_pPed, &packet->origin, &packet->effect, nullptr, &packet->target, nullptr);
	}
}

// PedPassengerSync

CPackets::PedPassengerSync* CPacketHandler::PedPassengerSync__Collect(CNetworkPed* networkPed, CNetworkVehicle* networkVehicle)
{
	CPackets::PedPassengerSync* packet = new CPackets::PedPassengerSync;

	packet->pedid = networkPed->m_nPedId;
	packet->vehicleid = networkVehicle->m_nVehicleId;
	packet->health = (unsigned char)networkPed->m_pPed->m_fHealth;
	packet->armour = (unsigned char)networkPed->m_pPed->m_fArmour;
	packet->weapon = networkPed->m_pPed->m_aWeapons[networkPed->m_pPed->m_nActiveWeaponSlot].m_eWeaponType;
	packet->ammo = networkPed->m_pPed->m_aWeapons[networkPed->m_pPed->m_nActiveWeaponSlot].m_nAmmoInClip;

	for (int i = 0; i < networkVehicle->m_pVehicle->m_nMaxPassengers; i++)
	{
		if(networkVehicle->m_pVehicle->m_apPassengers[i] == networkPed->m_pPed)
		{
			packet->seatid = i;
			break;
		}
	}

	return packet;
}

void CPacketHandler::PedPassengerSync__Handle(void* data, int size)
{
	CPackets::PedPassengerSync* packet = (CPackets::PedPassengerSync*)data;

	CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(packet->vehicleid);
	CNetworkPed* ped = CNetworkPedManager::GetPed(packet->pedid);

	if (vehicle == nullptr || ped == nullptr)
		return;

	if (vehicle->m_pVehicle == nullptr)
		return;

	if (ped->m_pPed == nullptr)
		return;

	if (!CUtil::IsValidEntityPtr(vehicle->m_pVehicle) || !CUtil::IsValidEntityPtr(ped->m_pPed))
		return;

	if (!ped->m_pPed->m_nPedFlags.bInVehicle || vehicle->m_pVehicle->m_pDriver == ped->m_pPed)
	{
		ped->WarpIntoVehiclePassenger(vehicle->m_pVehicle, packet->seatid);
	}

	CUtil::GiveWeaponByPacket(ped, packet->weapon, packet->ammo);

	ped->m_pPed->m_fArmour = packet->armour;
	ped->m_pPed->m_fHealth = packet->health;
	ped->m_fHealth = packet->health;
}

// PlayerAimSync

void CPacketHandler::PlayerAimSync__Trigger()
{
	CPackets::PlayerAimSync packet = CPacketHandler::PlayerAimSync__Collect();
	CNetwork::SendPacket(CPacketsID::PLAYER_AIM_SYNC, &packet, sizeof packet);
}

CPackets::PlayerAimSync CPacketHandler::PlayerAimSync__Collect()
{
	CPackets::PlayerAimSync packet{};

	CCam camera = TheCamera.m_aCams[TheCamera.m_nActiveCam];

	packet.cameraFov = camera.m_fFOV;
	packet.cameraMode = camera.m_nMode;
	packet.front = camera.m_vecFront;
	packet.source = camera.m_vecSource;
	packet.up = camera.m_vecUp;
	packet.moveHeading = CWorld::Players[0].m_PlayerData.m_fFPSMoveHeading;
	packet.aimY = CLocalPlayer::m_vecLastAimY;
	packet.aimZ = FindPlayerPed(0)->m_fAimingRotation;

	return packet;
}

void CPacketHandler::PlayerAimSync__Handle(void* data, int size)
{
	CPackets::PlayerAimSync* packet = (CPackets::PlayerAimSync*)data;

	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->playerid);

	if (player)
	{
		player->m_aimSyncData = *packet;
		if (player->m_lOnFoot)
		{
			player->m_lOnFoot->rotation = packet->aimZ;
		}
	}
}

// VehicleConfirm

void CPacketHandler::VehicleConfirm__Handle(void* data, int size)
{
	CPackets::VehicleConfirm* packet = (CPackets::VehicleConfirm*)data;

#ifdef PACKET_DEBUG_MESSAGES
	CChat::AddMessage("VEHICLE CONFIRM %d %d", packet->vehicleid, packet->tempid);
#endif

	unsigned char tempId = packet->tempid;
	
	if (tempId < 255)
	{
		auto tempVehicles = CNetworkVehicleManager::m_apTempVehicles;

		if (tempVehicles[tempId])
		{
			tempVehicles[tempId]->m_nVehicleId = packet->vehicleid;
			CNetworkVehicleManager::Add(tempVehicles[tempId]);
			tempVehicles[tempId] = nullptr;
		}
	}
}

// PedConfirm

void CPacketHandler::PedConfirm__Handle(void* data, int size)
{
	CPackets::PedConfirm* packet = (CPackets::PedConfirm*)data;

#ifdef PACKET_DEBUG_MESSAGES
	CChat::AddMessage("PED CONFIRM %d %d", packet->pedid, packet->tempid);
#endif

	unsigned char tempId = packet->tempid;

	if (tempId < 255)
	{
		auto tempPeds = CNetworkPedManager::m_apTempPeds;

		if (tempPeds[tempId])
		{
			tempPeds[tempId]->m_nPedId = packet->pedid;
			CNetworkPedManager::Add(tempPeds[tempId]);
			tempPeds[tempId] = nullptr;
		}
	}
}

// PlayerStats

void CPacketHandler::PlayerStats__Handle(void* data, int size)
{
	CPackets::PlayerStats* packet = (CPackets::PlayerStats*)data;
	
	if (auto networkPlayer = CNetworkPlayerManager::GetPlayer(packet->playerid))
	{
		for (size_t i = 0; i < CStatsSync::SYNCED_STATS_COUNT; i++)
		{
			eStats statId = CStatsSync::m_aeSyncedStats[i];
			networkPlayer->m_stats[statId] = packet->stats[i];
		}
	}
}

// RebuildPlayer

void CPacketHandler::RebuildPlayer__Handle(void* data, int size)
{
	CPackets::RebuildPlayer* packet = (CPackets::RebuildPlayer*)data;

	if (auto networkPlayer = CNetworkPlayerManager::GetPlayer(packet->playerid))
	{
		networkPlayer->m_stats[STAT_FAT] = packet->clothesData.m_fFatStat;
		networkPlayer->m_stats[STAT_MUSCLE] = packet->clothesData.m_fMuscleStat;

		CStatsSync::ApplyNetworkPlayerContext(networkPlayer);

		networkPlayer->m_pPedClothesDesc = packet->clothesData;

		if (auto player = networkPlayer->m_pPed)
		{
			*player->m_pPlayerData->m_pPedClothesDesc = packet->clothesData;
			if (player->m_pRwClump)
			{
				CClothes::RebuildPlayer(player, false);
			}
		}

		CStatsSync::ApplyLocalContext();
	}
}

void CPacketHandler::RebuildPlayer__Trigger()
{
	CPackets::RebuildPlayer packet{};
	packet.clothesData = *(FindPlayerPed(0)->m_pPlayerData->m_pPedClothesDesc);
	CNetwork::SendPacket(CPacketsID::REBUILD_PLAYER, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
}

// AssignVehicleSyncer

void CPacketHandler::AssignVehicleSyncer__Handle(void* data, int size)
{
	CPackets::AssignVehicleSyncer* packet = (CPackets::AssignVehicleSyncer*)data;
	
	CNetworkVehicle* networkVehicle = CNetworkVehicleManager::GetVehicle(packet->vehicleid);

	if (!networkVehicle)
		return;

	if (networkVehicle->m_bSyncing)
	{
#ifdef PACKET_DEBUG_MESSAGES
		CChat::AddMessage("NO MORE SYNCING THE VEHICLE %d", packet->vehicleid);
#endif
		networkVehicle->m_bSyncing = false;
	}
	else
	{
#ifdef PACKET_DEBUG_MESSAGES
		CChat::AddMessage("THE SERVER SAID ME TO SYNC THE VEHICLE %d", packet->vehicleid);
#endif
		networkVehicle->m_bSyncing = true;
	}
}

// RespawnPlayer

void CPacketHandler::RespawnPlayer__Handle(void* data, int size)
{
	CPackets::RespawnPlayer* packet = (CPackets::RespawnPlayer*)data;
	
	if (auto networkPlayer = CNetworkPlayerManager::GetPlayer(packet->playerid))
	{
		networkPlayer->Respawn();
	}
}

// MassPacketSequence

void CPacketHandler::MassPacketSequence__Handle(void* data, int size)
{
	if (size < 1)
	{
		CChat::AddMessage("Invalid mass packet data!");
		return;
	}

	char* buffer = static_cast<char*>(data);

	unsigned char packetCount = static_cast<unsigned char>(buffer[0]);
	int offset = 1;

	if (packetCount == 0 || offset >= size)
	{
		CChat::AddMessage("Invalid packet count or offset!");
		return;
	}

	for (unsigned char i = 0; i < packetCount; ++i)
	{
		if (offset + 2 > size)
		{
			CChat::AddMessage("Invalid packet data in mass packet!");
			return;
		}

		unsigned short packetId;
		memcpy(&packetId, buffer + offset, 2);
		offset += 2;

		size_t remainingData = size - offset;

		auto it = CNetwork::m_packetListeners.find(packetId);
		if (it != CNetwork::m_packetListeners.end())
		{
			it->second->m_callback(buffer + offset, (int)remainingData);
		}

		offset += CPackets::GetPacketSize((CPacketsID)packetId);
	}
}