#include "stdafx.h"
#include "CTaskSync.h"
#include "Entity/Types/CNetworkPed.h"
#include "Entity/Types/CNetworkVehicle.h"
#include "Entity/Types/CNetworkPlayer.h"

// PlayerConnected

void CPacketHandler::PlayerConnected__Handle(void* data, int size)
{
	// get packet struct
	CPackets::PlayerConnected* packet = (CPackets::PlayerConnected*)data;

	// create new player
	CNetworkPlayer* player = new CNetworkPlayer(packet->m_nPlayerId);

	CNetworkPlayerManager::Add(player);

	CChat::AddMessage("[Player] " + std::string(player->GetName()) + " connected");
}

// PlayerDisconnected

void CPacketHandler::PlayerDisconnected__Handle(void* data, int size)
{
	// get packet struct
	CPackets::PlayerDisconnected* packet = (CPackets::PlayerDisconnected*)data;

	// get player instance
	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->m_nPlayerId);

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
	
	CPackets::PlayerOnFoot* packet = new CPackets::PlayerOnFoot;

	packet->m_syncData.m_vecPosition = player->m_matrix->pos;
	packet->m_syncData.m_vecVelocity = player->m_vecMoveSpeed;
	packet->m_syncData.m_fRotation = player->m_fCurrentRotation;
	packet->m_syncData.m_nHealth = (uint8_t)player->m_fHealth;
	packet->m_syncData.m_nArmour = (uint8_t)player->m_fArmour;
	packet->m_syncData.m_nCurrentWeapon = player->m_aWeapons[player->m_nActiveWeaponSlot].m_eWeaponType;
	packet->m_syncData.m_nAmmoInClip = player->m_aWeapons[player->m_nActiveWeaponSlot].m_nAmmoInClip; // ammo in clip, not total
	packet->m_syncData.m_bDucking = CUtil::IsDucked(player);
	packet->m_syncData.m_fAimX = CLocalPlayer::m_fLastAimX;
	packet->m_syncData.m_fAimY = CLocalPlayer::m_fLastAimY;
	packet->m_syncData.m_bHasJetpack = CUtil::IsPedHasJetpack(player);

	return packet;
}

void CPacketHandler::PlayerOnFoot__Handle(void* data, int size)
{
	// get packet struct
	CPackets::PlayerOnFoot* packet = (CPackets::PlayerOnFoot*)data;

	// get player instance 
	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->m_nPlayerId);

	// check if player not connected
	if (player == nullptr)
		return;

	auto ped = player->GetEntity();

	if (CUtil::IsPositionUpdateNeeded(ped->m_matrix->pos, packet->m_syncData.m_vecPosition))
	{
		ped->m_matrix->pos = packet->m_syncData.m_vecPosition;
	}

	CUtil::GiveWeaponByPacket(player, packet->m_syncData.m_nCurrentWeapon, packet->m_syncData.m_nAmmoInClip);

	ped->m_fAimingRotation =
		ped->m_fCurrentRotation = packet->m_syncData.m_fRotation;

	CUtil::SetPlayerJetpack(player, packet->m_syncData.m_bHasJetpack);

	player->GetSyncData() = packet->m_syncData;
}

// PlayerBulletShot

void CPacketHandler::PlayerBulletShot__Handle(void* data, int size)
{
	CPackets::PlayerBulletShot* packet = (CPackets::PlayerBulletShot*)data;

	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->m_nPlayerId);

	CEntity* victim = nullptr;
	
	if (packet->m_nTargetId != -1)
	{

		CNetworkPlayer* damagedPlayer = CNetworkPlayerManager::GetPlayer(packet->m_nTargetId);
		CNetworkVehicle* damagedVehicle = CNetworkVehicleManager::GetVehicle(packet->m_nTargetId);

		if (damagedPlayer != nullptr)
			victim = damagedPlayer->m_pPed;

		if (damagedVehicle != nullptr)
			victim = damagedVehicle->m_pEntity;

		if (packet->m_nTargetId == CNetworkPlayerManager::m_nMyId && packet->entityType == eEntityType::ENTITY_TYPE_PED)
			victim = FindPlayerPed(0);
	}

	player->m_pEntity->m_aWeapons[player->m_pEntity->m_nActiveWeaponSlot].DoBulletImpact(player->m_pEntity, victim, &packet->startPos, &packet->endPos, &packet->colPoint, packet->incrementalHit);
}

// PlayerHandshake

void CPacketHandler::PlayerHandshake__Handle(void* data, int size)
{
	CPackets::PlayerHandshake* packet = (CPackets::PlayerHandshake*)data;

	CNetworkPlayerManager::m_nMyId = packet->m_nPlayerId;

	CPackets::PlayerGetName getNamePacket = {0};
	strcpy(getNamePacket.name, CLocalPlayer::m_name);

	CNetwork::SendPacket(ePacketType::PLAYER_GET_NAME, &getNamePacket, sizeof getNamePacket, ENET_PACKET_FLAG_RELIABLE);
}

// PlayerPlaceWaypoint

void CPacketHandler::PlayerPlaceWaypoint__Handle(void* data, int size)
{
	CPackets::PlayerPlaceWaypoint* packet = (CPackets::PlayerPlaceWaypoint*)data;

	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->m_nPlayerId);

	player->m_bWaypointPlaced = packet->m_bPlace;
	player->m_vecWaypointPos = packet->m_vecPosition;

#ifdef _DEV
	CChat::AddMessage("WAYPOINT PLACE %d %f %f\n", packet->m_bPlace, packet->m_vecPosition.x, packet->m_vecPosition.y);
#endif
}

// PlayerGetName

void CPacketHandler::PlayerGetName__Handle(void* data, int size)
{
	CPackets::PlayerGetName* packet = (CPackets::PlayerGetName*)data;
	
	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->m_nPlayerId);

	strcpy_s(player->m_name, packet->m_aName);

	CChat::AddMessage("[Player] player " + std::to_string(player->GetId()) + " now aka " + player->m_name);

	CPacketHandler::GameWeatherTime__Trigger();
}

// PlayerSetHost

void CPacketHandler::PlayerSetHost__Handle(void* data, int size)
{
	CPackets::PlayerSetHost* packet = (CPackets::PlayerSetHost*)data;

	if (packet->m_nPlayerId == CNetworkPlayerManager::m_nMyId)
	{
		CLocalPlayer::m_bIsHost = true;

		CPopulation::PedDensityMultiplier = 1.0f;
		patch::SetFloat(0x8A5B20, 1.0f);

		CNetworkPedManager::AssignHost();
		CChat::AddMessage("[Player] You are the host now");

		CPacketHandler::GameWeatherTime__Trigger();
		return;
	}
	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->m_nPlayerId);

	CChat::AddMessage("[Player] " + std::string(player->GetName()) + " is now the host");

	CLocalPlayer::m_bIsHost = false;
}

// AddExplosion

void CPacketHandler::AddExplosion__Handle(void* data, int size)
{
	CPackets::AddExplosion* packet = (CPackets::AddExplosion*)data;
	plugin::Call<0x736A50, CEntity*, CPed*, int, CVector2D, float, int, char, float, char>(nullptr, nullptr, 
		packet->m_nType, 
		CVector2D(packet->m_vecPosition.x, packet->m_vecPosition.y),
		packet->m_vecPosition.z, 
		packet->m_nTime, 
		packet->m_bUsesSound, 
		packet->m_fCameraShake, 
		packet->m_bVisible);
}

// VehicleSpawn

void CPacketHandler::VehicleSpawn__Handle(void* data, int size)
{
	CPackets::VehicleSpawn* packet = (CPackets::VehicleSpawn*)data;
	
#ifdef _DEV
	CChat::AddMessage("VEHICLE SPAWN %d %d %f %f %f %f %p", packet->m_nVehicleId, packet->m_nModelId, packet->m_vecPosition.x, packet->m_vecPosition.y, packet->m_vecPosition.z, packet->m_fRotation);
#endif

	CNetworkVehicle* vehicle = new CNetworkVehicle
	(
		packet->m_nVehicleId,
		packet->m_nModelId
	);

	CNetworkVehicleManager::Add(vehicle);
}

// VehicleRemove

void CPacketHandler::VehicleRemove__Handle(void* data, int size)
{
	CPackets::VehicleRemove* packet = (CPackets::VehicleRemove*)data;

#ifdef _DEV
	char buffer[128];
	sprintf(buffer, "VEHICLE REMOVE %d", packet->m_nVehicleId);
	CChat::AddMessage(buffer);
#endif
	CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(packet->m_nVehicleId);

	CNetworkVehicleManager::Remove(vehicle);
	delete vehicle;
}

// VehicleIdleUpdate

CPackets::VehicleIdleUpdate* CPacketHandler::VehicleIdleUpdate__Collect(CNetworkVehicle* vehicle)
{
	CPackets::VehicleIdleUpdate* packet = new CPackets::VehicleIdleUpdate;
	packet->m_nVehicleId = vehicle->GetId();
	packet->m_vecPosition = vehicle->m_pEntity->m_matrix->pos;
	packet->m_vecRoll = vehicle->m_pEntity->m_matrix->right;
	packet->m_vecRotation = vehicle->m_pEntity->m_matrix->up;
	packet->m_vecVelocity = vehicle->m_pEntity->m_vecMoveSpeed;
	packet->m_nPrimaryColor = vehicle->m_pEntity->m_nPrimaryColor;
	packet->m_nSecondaryColor = vehicle->m_pEntity->m_nSecondaryColor;
	packet->m_nHealth = vehicle->m_pEntity->m_fHealth;

	if (CUtil::GetVehicleType(vehicle->m_pEntity) == eVehicleType::VEHICLE_PLANE)
	{
		CPlane* plane = (CPlane*)vehicle->m_pEntity;
		packet->m_fPlaneGearState = plane->m_fLandingGearStatus > 0.0f ? 1.0f : 0.0f;
	}

	packet->m_bLocked = vehicle->m_pEntity->m_eDoorLock;

	return packet;
}

void CPacketHandler::VehicleIdleUpdate__Handle(void* data, int size)
{
	CPackets::VehicleIdleUpdate* packet = (CPackets::VehicleIdleUpdate*)data;
	CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(packet->m_nVehicleId);

	if (vehicle == nullptr)
		return;

	/*if (vehicle->m_pEntity == nullptr)
		vehicle->CreateVehicle(vehicle->m_nVehicleId, vehicle->m_nModelId, packet->pos, 0.f, packet->color1, packet->color2);*/

	vehicle->m_pEntity->m_matrix->pos = packet->m_vecPosition;		   
	vehicle->m_pEntity->m_matrix->right = packet->m_vecRoll;	   
	vehicle->m_pEntity->m_matrix->up = packet->m_vecRotation;		   
	vehicle->m_pEntity->m_vecMoveSpeed = packet->m_vecVelocity;

	vehicle->m_pEntity->m_nPrimaryColor = packet->m_nPrimaryColor;
	vehicle->m_pEntity->m_nSecondaryColor = packet->m_nSecondaryColor;
	vehicle->m_pEntity->m_fHealth = packet->m_nHealth;

	if (CUtil::GetVehicleType(vehicle->m_pEntity) == eVehicleType::VEHICLE_PLANE)
	{
		CPlane* plane = (CPlane*)vehicle->m_pEntity;

		plane->m_fLandingGearStatus = packet->m_fPlaneGearState;
	}

	vehicle->m_pEntity->m_eDoorLock = (eDoorLock)packet->m_bLocked;
}

// VehicleDriverUpdate

CPackets::VehicleDriverUpdate* CPacketHandler::VehicleDriverUpdate__Collect(CNetworkVehicle* vehicle)
{
	CPackets::VehicleDriverUpdate* packet = new CPackets::VehicleDriverUpdate;
	CPlayerPed* player = FindPlayerPed(0);

	// vehicle data
	packet->m_nVehicleId =		vehicle->GetId();
	packet->m_syncData.m_vecPosition =				vehicle->m_pEntity->m_matrix->pos;
	packet->m_syncData.m_vecRoll=				vehicle->m_pEntity->m_matrix->right;
	packet->m_syncData.m_vecRotation =				vehicle->m_pEntity->m_matrix->up;
	packet->m_syncData.m_vecVelocity =			vehicle->m_pEntity->m_vecMoveSpeed;

	// player data
	packet->ammo =				player->m_aWeapons[player->m_nActiveWeaponSlot].m_nAmmoInClip;
	packet->playerArmour =		(unsigned char)player->m_fArmour;
	packet->playerHealth =		(unsigned char)player->m_fHealth;
	packet->weapon =			player->m_aWeapons[player->m_nActiveWeaponSlot].m_eWeaponType;

	packet->color1 = vehicle->m_pEntity->m_nPrimaryColor;
	packet->color2 = vehicle->m_pEntity->m_nSecondaryColor;

	packet->health = vehicle->m_pEntity->m_fHealth;

	packet->paintjob = vehicle->m_nPaintJob;

	if (CUtil::IsVehicleHasTurret(vehicle->m_pEntity))
	{
		CAutomobile* automobile = (CAutomobile*)vehicle->m_pEntity;
		packet->turretAimHorizontal = automobile->m_fDoomHorizontalRotation;
		packet->turretAimVertical = automobile->m_fDoomVerticalRotation;
	}

	if (vehicle->m_pEntity->m_nModelIndex == 520)
	{
		CAutomobile* automobile = (CAutomobile*)vehicle->m_pEntity;
		packet->miscComponentAngle = automobile->m_wMiscComponentAngle;
	}

	eVehicleType vehicleType = CUtil::GetVehicleType(vehicle->m_pEntity);
	if (vehicleType == eVehicleType::VEHICLE_BIKE || vehicleType == eVehicleType::VEHICLE_BMX)
	{
		CBike* bike = (CBike*)vehicle->m_pEntity;

		packet->bikeLean = bike->m_rideAnimData.m_fAnimLean;
	}

	if (vehicleType == eVehicleType::VEHICLE_PLANE)
	{
		CPlane* plane = (CPlane*)vehicle->m_pEntity;
		packet->planeGearState = plane->m_fLandingGearStatus;
	}

	packet->locked = vehicle->m_pEntity->m_eDoorLock;

	return packet;
}

void CPacketHandler::VehicleDriverUpdate__Handle(void* data, int size)
{
	CPackets::VehicleDriverUpdate* packet = (CPackets::VehicleDriverUpdate*)data;

	CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(packet->m_nVehicleId);
	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->m_nPlayerId);

	if (vehicle == nullptr || player == nullptr)
		return;

	if (vehicle->m_pEntity == nullptr)
	{
		vehicle->CreateVehicle(vehicle->m_nVehicleId, vehicle->m_nModelId, packet->pos, 0.f, packet->color1, packet->color2);
		return;
	}

	if (player->m_pEntity == nullptr)
	{
		player->CreatePed(player->m_nPlayerId, packet->pos);
		return;
	}

	if (player->m_pEntity->m_pEntity != vehicle->m_pEntity)
		plugin::Command<Commands::WARP_CHAR_INTO_CAR>(CPools::GetPedRef(player->m_pEntity), CPools::GetVehicleRef(vehicle->m_pEntity));

	vehicle->m_pEntity->m_matrix->pos = packet->pos;
	vehicle->m_pEntity->m_matrix->right = packet->roll;
	vehicle->m_pEntity->m_matrix->up = packet->rot;
	vehicle->m_pEntity->m_vecMoveSpeed = packet->velocity;
	
	CUtil::GiveWeaponByPacket(player, packet->weapon, packet->ammo);

	player->m_oOnFoot = player->m_lOnFoot;
	
	player->m_lOnFoot->armour = packet->playerArmour;
	player->m_lOnFoot->health = packet->playerHealth;

	vehicle->m_pEntity->m_nPrimaryColor = packet->color1;
	vehicle->m_pEntity->m_nSecondaryColor = packet->color2;

	vehicle->m_pEntity->m_fHealth = packet->health;

	if(vehicle->m_nPaintJob != packet->paintjob)
		vehicle->m_pEntity->SetRemap(packet->paintjob);

	vehicle->m_fAimHorizontal = packet->turretAimHorizontal;
	vehicle->m_fAimVertical = packet->turretAimVertical;

	if (vehicle->m_pEntity->m_nModelIndex == 520)
	{
		CAutomobile* automobile = (CAutomobile*)vehicle->m_pEntity;
		automobile->m_wMiscComponentAngle = packet->miscComponentAngle;
	}

	eVehicleType vehicleType = CUtil::GetVehicleType(vehicle->m_pEntity);
	if (vehicleType == eVehicleType::VEHICLE_BIKE || vehicleType == eVehicleType::VEHICLE_BMX)
	{
		CBike* bike = (CBike*)vehicle->m_pEntity;

		*(float*)((DWORD)&*bike + 0x64C) = packet->m_syncData.m_fBikeLean;
	}

	if (vehicleType == eVehicleType::VEHICLE_PLANE)
	{
		CPlane* plane = (CPlane*)vehicle->m_pEntity;

		plane->m_fLandingGearStatus = packet->planeGearState;
	}

	vehicle->m_pEntity->m_eDoorLock = (eDoorLock)packet->locked;
}

// VehicleEnter

void CPacketHandler::VehicleEnter__Handle(void* data, int size)
{
	CPackets::VehicleEnter* packet = (CPackets::VehicleEnter*)data;
	
	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->m_nPlayerId);
	CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(packet->m_nVehicleId);

	if (player == nullptr || vehicle == nullptr)
	{
		return;
	}

#ifdef _DEV
	CChat::AddMessage("player %d entered vehicleid %d %s", packet->m_nPlayerId, packet->m_nVehicleId, packet->seatid != 0 ? "as passenger" : "");
#endif

	if (packet->seatid == 0) // driver
	{
		if (packet->force)
		{
			for (unsigned char i = 0; i < 5; i++)
			{
				if (vehicle->m_pEntity)
					break;

				Sleep(100);
			}

			plugin::Command<Commands::WARP_CHAR_INTO_CAR>(CPools::GetPedRef(player->m_pEntity), CPools::GetVehicleRef(vehicle->m_pEntity));
		}
		else
			plugin::Command<Commands::TASK_ENTER_CAR_AS_DRIVER>(CPools::GetPedRef(player->m_pEntity), CPools::GetVehicleRef(vehicle->m_pEntity), 3000);
	}
	else
	{
		//plugin::Command<Commands::WARP_CHAR_INTO_CAR_AS_PASSENGER>(CPools::GetPedRef(player->m_pEntity), CPools::GetVehicleRef(vehicle->m_pEntity), -1);
		plugin::Command<0x5CA>(CPools::GetPedRef(player->m_pEntity), CPools::GetVehicleRef(vehicle->m_pEntity), 3000, -1);
	}
}

// VehicleExit

void CPacketHandler::VehicleExit__Handle(void* data, int size)
{
	CPackets::VehicleExit* packet = (CPackets::VehicleExit*)data;

	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->m_nPlayerId);

	if (player == nullptr)
	{
		return;
	}

	if (player->m_pEntity->m_pEntity == nullptr)
	{
		return;
	}
#ifdef _DEV
	CChat::AddMessage("player %d exited from vehicle", packet->m_nPlayerId);
#endif
	if (packet->force)
	{
		CVector doorPos{};
		player->m_pEntity->m_pEntity->GetComponentWorldPosition(8, doorPos); // right front door (driver)
		plugin::Command<Commands::WARP_CHAR_FROM_CAR_TO_COORD>(CPools::GetPedRef(player->m_pEntity), doorPos.x, doorPos.y, doorPos.z);
	}
	else
		plugin::Command<Commands::TASK_LEAVE_CAR>(CPools::GetPedRef(player->m_pEntity), CPools::GetVehicleRef(player->m_pEntity->m_pEntity));
}

// VehicleDamage

void CPacketHandler::VehicleDamage__Handle(void* data, int size)
{
#ifdef _DEV
	CChat::AddMessage("VehicleDamage__Handle");
#endif
	CPackets::VehicleDamage* packet = (CPackets::VehicleDamage*)data;

	CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(packet->m_nVehicleId);
	*(CDamageManager*)((DWORD)vehicle->m_pEntity + 0x5A0) = packet->damageManager;

	DWORD dwVehiclePtr = (DWORD)vehicle->m_pEntity;
	_asm mov ecx, dwVehiclePtr
	_asm mov edx, 0x6B3E90 // CAutomobile::UpdateDamageModel
	_asm call edx
}

// VehicleComponentAdd

void CPacketHandler::VehicleComponentAdd__Handle(void* data, int size)
{
	CPackets::VehicleComponentAdd* packet = (CPackets::VehicleComponentAdd*)data;

	CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(packet->m_nVehicleId);

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

	vehicle->m_pEntity->AddVehicleUpgrade(packet->componentid);
}

// VehicleComponentRemove

void CPacketHandler::VehicleComponentRemove__Handle(void* data, int size)
{
	CPackets::VehicleComponentAdd* packet = (CPackets::VehicleComponentAdd*)data;

	CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(packet->m_nVehicleId);

	if (vehicle == nullptr)
		return;

	vehicle->m_pEntity->RemoveVehicleUpgrade(packet->componentid);
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
	packet->m_nVehicleId = vehicle->m_nVehicleId;
	packet->driveby = CDriveBy::IsPedInDriveby(localPlayer);
	packet->aim = *(CVector*)0xB6F32C;

	return packet;
}

void CPacketHandler::VehiclePassengerUpdate__Handle(void* data, int size)
{
	CPackets::VehiclePassengerUpdate* packet = (CPackets::VehiclePassengerUpdate*)data;

	CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(packet->m_nVehicleId);
	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->m_nPlayerId);

	if (vehicle == nullptr || player == nullptr)
		return;

	if (vehicle->m_pEntity == nullptr)
		return;

	if (player->m_pEntity == nullptr)
		return;

	if (!player->m_pEntity->m_nPedFlags.bInVehicle || (player->m_pEntity->m_nPedFlags.bInVehicle && vehicle->m_pEntity->m_pDriver == player->m_pEntity))
	{
#ifdef _DEV
		CChat::AddMessage("forcing enter passenger %d", player->m_nPlayerId);
#endif
		plugin::Command<Commands::WARP_CHAR_INTO_CAR_AS_PASSENGER>(CPools::GetPedRef(player->m_pEntity), CPools::GetVehicleRef(vehicle->m_pEntity), -1);
	}

	CUtil::GiveWeaponByPacket(player, packet->weapon, packet->ammo);

	player->m_oOnFoot = player->m_lOnFoot;

	player->m_pEntity->m_fArmour = player->m_lOnFoot->armour = packet->playerArmour;
	player->m_pEntity->m_fHealth = player->m_lOnFoot->health = packet->playerHealth;
	player->m_aPassengerAim = packet->aim;

	if (packet->driveby && !CDriveBy::IsPedInDriveby(player->m_pEntity))
	{
		CDriveBy::StartDriveby(player->m_pEntity);
	}
	else if (!packet->driveby && CDriveBy::IsPedInDriveby(player->m_pEntity))
	{
		CDriveBy::StopDriveby(player->m_pEntity);
	}
}

// PlayerChatMessage

void CPacketHandler::PlayerChatMessage__Handle(void* data, int size)
{
	CPackets::PlayerChatMessage* packet = (CPackets::PlayerChatMessage*)data;
	
	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->m_nPlayerId);

	if (player)
	{
		CChat::AddMessage("%s(%d): %s", player->GetName(), player->m_nPlayerId, packet->message);
	}
}

// PedSpawn

void CPacketHandler::PedSpawn__Handle(void* data, int size)
{
	CPackets::PedSpawn* packet = (CPackets::PedSpawn*)data;

#ifdef _DEV
	CChat::AddMessage("PED SPAWN %d %d %f %f %f %d %d", packet->pedid, packet->modelId, packet->pos.x, packet->pos.y, packet->pos.z, packet->pedType, packet->createdBy);
#endif

	CNetworkPed* ped = new CNetworkPed(packet->pedid, (int)packet->modelId, (ePedType)packet->pedType, packet->pos, packet->createdBy);

	CNetworkPedManager::Add(ped);
}

// PedRemove

void CPacketHandler::PedRemove__Handle(void* data, int size)
{
	CPackets::PedRemove* packet = (CPackets::PedRemove*)data;

#ifdef _DEV
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

	if (ped->m_pPed->m_pEntity && ped->m_pPed->m_nPedFlags.bInVehicle)
	{
		//plugin::Command<Commands::TASK_LEAVE_CAR>(CPools::GetPedRef(ped->m_pPed), CPools::GetVehicleRef(ped->m_pPed->m_pEntity));
		plugin::Command<Commands::WARP_CHAR_FROM_CAR_TO_COORD>(CPools::GetPedRef(ped->m_pPed), packet->pos.x, packet->pos.y, packet->pos.z);
	}

	CUtil::GiveWeaponByPacket(ped, packet->weapon, packet->ammo);

	ped->m_pPed->m_matrix->pos = packet->pos;
	ped->m_fCurrentRotation = ped->m_pPed->m_fCurrentRotation = packet->currentRotation;
	ped->m_fAimingRotation = ped->m_pPed->m_fAimingRotation = packet->aimingRotation;
	ped->m_fLookDirection = ped->m_pPed->field_73C = packet->lookDirection;
	ped->m_pPed->m_fHealth = packet->health;
	ped->m_pPed->m_fArmour = packet->armour;
	ped->m_vecVelocity = packet->velocity;
	ped->m_nMoveState = (eMoveState)packet->moveState;
}

// GameWeatherTime

CPackets::GameWeatherTime* CPacketHandler::GameWeatherTime__Collect()
{
	CPackets::GameWeatherTime* packet = new CPackets::GameWeatherTime;
	packet->newWeather = CWeather::NewWeatherType;
	packet->oldWeather = CWeather::OldWeatherType;
	packet->forcedWeather = CWeather::ForcedWeatherType;
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

	CWeather::NewWeatherType = packet->newWeather;
	CWeather::OldWeatherType = packet->oldWeather;
	CWeather::ForcedWeatherType = packet->forcedWeather;
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
	CNetwork::SendPacket(ePacketType::GAME_WEATHER_TIME, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE);
}

// PlayerKeySync

void CPacketHandler::PlayerKeySync__Handle(void* data, int size)
{
	CPackets::PlayerKeySync* packet = (CPackets::PlayerKeySync*)data;

	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->m_nPlayerId);
	player->m_compressedControllerState = packet->newState;
}

// PedAddTask

void CPacketHandler::PedAddTask__Handle(void* data, int size)
{
	CTaskSync::DeSerializeTask(data);
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
	packet->m_nVehicleId = vehicle->m_nVehicleId;
	packet->pos = vehicle->m_pEntity->m_matrix->pos;
	packet->roll = vehicle->m_pEntity->m_matrix->right;
	packet->rot = vehicle->m_pEntity->m_matrix->up;
	packet->velocity = vehicle->m_pEntity->m_vecMoveSpeed;

	packet->color1 = vehicle->m_pEntity->m_nPrimaryColor;
	packet->color2 = vehicle->m_pEntity->m_nSecondaryColor;

	packet->health = vehicle->m_pEntity->m_fHealth;

	packet->paintjob = vehicle->m_nPaintJob;

	eVehicleType vehicleType = CUtil::GetVehicleType(vehicle->m_pEntity);
	if (vehicleType == eVehicleType::VEHICLE_BIKE || vehicleType == eVehicleType::VEHICLE_BMX)
	{
		CBike* bike = (CBike*)vehicle->m_pEntity;

		packet->bikeLean = bike->m_rideAnimData.m_fAnimLean;
	}

	if (vehicleType == eVehicleType::VEHICLE_PLANE)
	{
		CPlane* plane = (CPlane*)vehicle->m_pEntity;
		packet->planeGearState = plane->m_fLandingGearStatus;
	}

	packet->locked = vehicle->m_pEntity->m_eDoorLock;
	
	packet->autoPilot = CSyncAutoPilot(vehicle->m_pEntity->m_autoPilot);

	packet->gasPedal = vehicle->m_pEntity->m_fGasPedal;
	packet->breakPedal = vehicle->m_pEntity->m_fBreakPedal;
	packet->steerAngle = vehicle->m_pEntity->m_fSteerAngle;

	return packet;
}

void CPacketHandler::PedDriverUpdate__Handle(void* data, int size)
{
	CPackets::PedDriverUpdate* packet = (CPackets::PedDriverUpdate*)data;

	CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(packet->m_nVehicleId);
	CNetworkPed* ped = CNetworkPedManager::GetPed(packet->pedid);

	if (vehicle == nullptr || ped == nullptr || ped->m_pPed == nullptr)
		return;

	if (vehicle->m_pEntity == nullptr)
	{
		vehicle->CreateVehicle(vehicle->m_nVehicleId, vehicle->m_nModelId, packet->pos, 0.f, packet->color1, packet->color2);
		return;
	}

	if (ped->m_pPed->m_pEntity != vehicle->m_pEntity)
		plugin::Command<Commands::WARP_CHAR_INTO_CAR>(CPools::GetPedRef(ped->m_pPed), CPools::GetVehicleRef(vehicle->m_pEntity));

	if(CUtil::IsPositionUpdateNeeded(packet->pos, vehicle->m_pEntity->m_matrix->pos))
		vehicle->m_pEntity->m_matrix->pos = packet->pos;

	vehicle->m_pEntity->m_matrix->right = packet->roll;
	vehicle->m_pEntity->m_matrix->up = packet->rot;
	ped->m_vecVelocity = packet->velocity;
	vehicle->m_pEntity->m_vecMoveSpeed = packet->velocity;

	CUtil::GiveWeaponByPacket(ped, packet->weapon, packet->ammo);

	ped->m_pPed->m_fArmour = packet->pedArmour;
	ped->m_pPed->m_fHealth = packet->pedHealth;

	vehicle->m_pEntity->m_nPrimaryColor = packet->color1;
	vehicle->m_pEntity->m_nSecondaryColor = packet->color2;

	vehicle->m_pEntity->m_fHealth = packet->health;

	if (vehicle->m_nPaintJob != packet->paintjob)
		vehicle->m_pEntity->SetRemap(packet->paintjob);

	eVehicleType vehicleType = CUtil::GetVehicleType(vehicle->m_pEntity);
	if (vehicleType == eVehicleType::VEHICLE_BIKE || vehicleType == eVehicleType::VEHICLE_BMX)
	{
		CBike* bike = (CBike*)vehicle->m_pEntity;

		*(float*)((DWORD)&*bike + 0x64C) = packet->bikeLean;
	}

	if (CUtil::GetVehicleType(vehicle->m_pEntity) == eVehicleType::VEHICLE_PLANE)
	{
		CPlane* plane = (CPlane*)vehicle->m_pEntity;

		plane->m_fLandingGearStatus = packet->planeGearState;
	}

	vehicle->m_pEntity->m_eDoorLock = (eDoorLock)packet->locked;
	packet->autoPilot.WriteTo(vehicle->m_pEntity->m_autoPilot);
	ped->m_autoPilot = vehicle->m_pEntity->m_autoPilot;

	ped->m_fGasPedal = packet->gasPedal;
	ped->m_fBreakPedal = packet->breakPedal;
	ped->m_fSteerAngle = packet->steerAngle;
}

// EntityStream

void CPacketHandler::EntityStream__Handle(void* data, int size)
{
	CPackets::EntityStream* packet = (CPackets::EntityStream*)data;
	
	switch (packet->entityType)
	{
	case eNetworkEntityType::NETWORK_ENTITY_PLAYER:
	{
		if (auto player = CNetworkPlayerManager::GetPlayer(packet->entityid))
		{
			if (packet->in)
				player->StreamIn(player->m_lOnFoot->position);
			else
				player->StreamOut();
		}

		break;
	}
	case eNetworkEntityType::NETWORK_ENTITY_VEHICLE:
	case eNetworkEntityType::NETWORK_ENTITY_PED:
		break;
	}
}