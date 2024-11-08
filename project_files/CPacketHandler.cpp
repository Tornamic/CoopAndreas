#include "stdafx.h"
#include "CTaskSync.h"
#include "CPacketHandler.h"
#include "Entity/Types/CNetworkPed.h"
#include "Entity/Types/CNetworkVehicle.h"
#include "Entity/Types/CNetworkPlayer.h"
#include "Entity/Manager/Types/CNetworkPlayerManager.h"
#include "Entity/Manager/Types/CNetworkPedManager.h"
#include "Entity/Manager/Types/CNetworkVehicleManager.h"
#include "CChat.h"
#include "CUtil.h"
#include "CLocalPlayer.h"
#include "CNetwork.h"
#include "CDriveBy.h"

// PlayerConnected

void CPacketHandler::PlayerConnected__Handle(void* data, int size)
{
	// get packet struct
	CPackets::PlayerConnected* packet = (CPackets::PlayerConnected*)data;

	// create new player
	CNetworkPlayer* player = new CNetworkPlayer(packet->m_nPlayerId);

	CNetworkPlayerManager::Instance().Add(player);

	CChat::AddMessage("[Player] " + std::string(player->GetName()) + " connected");
}

// PlayerDisconnected

void CPacketHandler::PlayerDisconnected__Handle(void* data, int size)
{
	// get packet struct
	CPackets::PlayerDisconnected* packet = (CPackets::PlayerDisconnected*)data;

	// get player instance
	CNetworkPlayer* player = CNetworkPlayerManager::Instance().Get(packet->m_nPlayerId);

	if (player == nullptr)
		return;

	CChat::AddMessage("[Player] " + std::string(player->GetName()) + " disconnected");

	// remove from list
	CNetworkPlayerManager::Instance().Remove(player);

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
	CNetworkPlayer* player = CNetworkPlayerManager::Instance().Get(packet->m_nPlayerId);

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

	CNetworkPlayer* player = CNetworkPlayerManager::Instance().Get(packet->m_nPlayerId);

	CEntity* victim = nullptr;
	
	if (packet->m_nTargetId != -1)
	{

		CNetworkPlayer* damagedPlayer = CNetworkPlayerManager::Instance().Get(packet->m_nTargetId);
		CNetworkVehicle* damagedVehicle = CNetworkVehicleManager::Instance().Get(packet->m_nTargetId);

		if (damagedPlayer != nullptr)
			victim = damagedPlayer->m_pEntity;

		if (damagedVehicle != nullptr)
			victim = damagedVehicle->m_pEntity;

		if (packet->m_nTargetId == CLocalPlayer::m_nMyId && packet->m_nTargetEntityType == eNetworkEntityType::NETWORK_ENTITY_PED)
			victim = FindPlayerPed(0);
	}

	player->m_pEntity->m_aWeapons[player->m_pEntity->m_nActiveWeaponSlot].DoBulletImpact(player->m_pEntity, victim, &packet->m_vecStartPos, &packet->m_vecEndPos, &packet->m_colPoint, packet->m_nIncrementalHit);
}

// PlayerHandshake

void CPacketHandler::PlayerHandshake__Handle(void* data, int size)
{
	CPackets::PlayerHandshake* packet = (CPackets::PlayerHandshake*)data;

	CLocalPlayer::m_nMyId = packet->m_nPlayerId;

	CPackets::PlayerGetName getNamePacket = {0};
	strcpy(getNamePacket.m_aName, CLocalPlayer::m_name);

	CNetwork::SendPacket(ePacketType::PLAYER_GET_NAME, &getNamePacket, sizeof getNamePacket, ENET_PACKET_FLAG_RELIABLE);
}

// PlayerPlaceWaypoint

void CPacketHandler::PlayerPlaceWaypoint__Handle(void* data, int size)
{
	CPackets::PlayerPlaceWaypoint* packet = (CPackets::PlayerPlaceWaypoint*)data;

	CNetworkPlayer* player = CNetworkPlayerManager::Instance().Get(packet->m_nPlayerId);

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
	
	CNetworkPlayer* player = CNetworkPlayerManager::Instance().Get(packet->m_nPlayerId);

	strcpy_s(player->m_name, packet->m_aName);

	CChat::AddMessage("[Player] player " + std::to_string(player->GetId()) + " now aka " + player->m_name);

	CPacketHandler::GameWeatherTime__Trigger();
}

// PlayerSetHost

void CPacketHandler::PlayerSetHost__Handle(void* data, int size)
{
	CPackets::PlayerSetHost* packet = (CPackets::PlayerSetHost*)data;

	if (packet->m_nPlayerId == CLocalPlayer::m_nMyId)
	{
		CLocalPlayer::m_bIsHost = true;

		CPopulation::PedDensityMultiplier = 1.0f;
		patch::SetFloat(0x8A5B20, 1.0f);

		CNetworkPedManager::Instance().AssignHost();
		CChat::AddMessage("[Player] You are the host now");

		CPacketHandler::GameWeatherTime__Trigger();
		return;
	}
	CNetworkPlayer* player = CNetworkPlayerManager::Instance().Get(packet->m_nPlayerId);

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

	CNetworkVehicleManager::Instance().Add(vehicle);
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
	CNetworkVehicle* vehicle = CNetworkVehicleManager::Instance().Get(packet->m_nVehicleId);

	CNetworkVehicleManager::Instance().Remove(vehicle);
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
	CNetworkVehicle* vehicle = CNetworkVehicleManager::Instance().Get(packet->m_nVehicleId);

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
	packet->m_nAmmoInClip =			player->m_aWeapons[player->m_nActiveWeaponSlot].m_nAmmoInClip;
	packet->m_nPlayerArmour =		(unsigned char)player->m_fArmour;
	packet->m_nPlayerHealth =		(unsigned char)player->m_fHealth;
	packet->m_nCurrentWeapon =		player->m_aWeapons[player->m_nActiveWeaponSlot].m_eWeaponType;

	packet->m_syncData.m_nPrimaryColor = vehicle->m_pEntity->m_nPrimaryColor;
	packet->m_syncData.m_nSecondaryColor = vehicle->m_pEntity->m_nSecondaryColor;

	packet->m_syncData.m_nHealth = vehicle->m_pEntity->m_fHealth;

	packet->m_syncData.m_nPaintjob = vehicle->m_nPaintJob;

	if (CUtil::IsVehicleHasTurret(vehicle->m_pEntity))
	{
		CAutomobile* automobile = (CAutomobile*)vehicle->m_pEntity;
		packet->m_syncData.m_fTurretAimHorizontal = automobile->m_fDoomHorizontalRotation;
		packet->m_syncData.m_fTurretAimVertical = automobile->m_fDoomVerticalRotation;
	}

	if (vehicle->m_pEntity->m_nModelIndex == 520)
	{
		CAutomobile* automobile = (CAutomobile*)vehicle->m_pEntity;
		packet->m_syncData.m_nMiscComponentAngle = automobile->m_wMiscComponentAngle;
	}

	eVehicleType vehicleType = CUtil::GetVehicleType(vehicle->m_pEntity);
	if (vehicleType == eVehicleType::VEHICLE_BIKE || vehicleType == eVehicleType::VEHICLE_BMX)
	{
		CBike* bike = (CBike*)vehicle->m_pEntity;

		packet->m_syncData.m_fBikeLean = bike->m_rideAnimData.m_fAnimLean;
	}

	if (vehicleType == eVehicleType::VEHICLE_PLANE)
	{
		CPlane* plane = (CPlane*)vehicle->m_pEntity;
		packet->m_syncData.m_fPlaneGearState = plane->m_fLandingGearStatus;
	}

	packet->m_syncData.m_nDoorLock = vehicle->m_pEntity->m_eDoorLock;

	return packet;
}

void CPacketHandler::VehicleDriverUpdate__Handle(void* data, int size)
{
	CPackets::VehicleDriverUpdate* packet = (CPackets::VehicleDriverUpdate*)data;

	CNetworkVehicle* vehicle = CNetworkVehicleManager::Instance().Get(packet->m_nVehicleId);
	CNetworkPlayer* player = CNetworkPlayerManager::Instance().Get(packet->m_nPlayerId);

	if (vehicle == nullptr || player == nullptr)
		return;

	if (vehicle->m_pEntity == nullptr)
	{
		//vehicle->Create(vehicle->m_nVehicleId, vehicle->m_nModelId, packet->pos, 0.f, packet->color1, packet->color2);
		return;
	}

	if (player->m_pEntity == nullptr)
	{
		//player->CreatePed(player->m_nPlayerId, packet->pos);
		return;
	}

	if (player->m_pEntity->m_pVehicle != vehicle->m_pEntity)
		plugin::Command<Commands::WARP_CHAR_INTO_CAR>(CPools::GetPedRef(player->m_pEntity), CPools::GetVehicleRef(vehicle->m_pEntity));

	vehicle->m_pEntity->m_matrix->pos = packet->m_syncData.m_vecPosition;
	vehicle->m_pEntity->m_matrix->right = packet->m_syncData.m_vecRoll;
	vehicle->m_pEntity->m_matrix->up = packet->m_syncData.m_vecRotation;
	vehicle->m_pEntity->m_vecMoveSpeed = packet->m_syncData.m_vecVelocity;
	
	CUtil::GiveWeaponByPacket(player, packet->m_nCurrentWeapon, packet->m_nAmmoInClip);

	auto& syncData = player->GetSyncData();
	syncData.m_nAmmoInClip = packet->m_nAmmoInClip;
	syncData.m_nArmour = packet->m_nPlayerArmour;
	syncData.m_nHealth = packet->m_nPlayerHealth;
	syncData.m_nCurrentWeapon = packet->m_nCurrentWeapon;

	vehicle->m_pEntity->m_nPrimaryColor = packet->m_syncData.m_nPrimaryColor;
	vehicle->m_pEntity->m_nSecondaryColor = packet->m_syncData.m_nSecondaryColor;

	vehicle->m_pEntity->m_fHealth = packet->m_syncData.m_nHealth;

	if(vehicle->m_nPaintJob != packet->m_syncData.m_nPaintjob)
		vehicle->m_pEntity->SetRemap(packet->m_syncData.m_nPaintjob);

	vehicle->m_fAimHorizontal = packet->m_syncData.m_fTurretAimHorizontal;
	vehicle->m_fAimVertical = packet->m_syncData.m_fTurretAimVertical;

	if (vehicle->m_pEntity->m_nModelIndex == 520)
	{
		CAutomobile* automobile = (CAutomobile*)vehicle->m_pEntity;
		automobile->m_wMiscComponentAngle = packet->m_syncData.m_nMiscComponentAngle;
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

		plane->m_fLandingGearStatus = packet->m_syncData.m_fPlaneGearState;
	}

	vehicle->m_pEntity->m_eDoorLock = (eDoorLock)packet->m_syncData.m_nDoorLock;
}

// VehicleEnter

void CPacketHandler::VehicleEnter__Handle(void* data, int size)
{
	CPackets::VehicleEnter* packet = (CPackets::VehicleEnter*)data;
	
	CNetworkPlayer* player = CNetworkPlayerManager::Instance().Get(packet->m_nPlayerId);
	CNetworkVehicle* vehicle = CNetworkVehicleManager::Instance().Get(packet->m_nVehicleId);

	if (player == nullptr || vehicle == nullptr)
	{
		return;
	}

#ifdef _DEV
	CChat::AddMessage("player %d entered vehicleid %d %s", packet->m_nPlayerId, packet->m_nVehicleId, packet->m_nSeatId != 0 ? "as passenger" : "");
#endif

	if (packet->m_nSeatId == 0) // driver
	{
		if (packet->m_bForce)
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

	CNetworkPlayer* player = CNetworkPlayerManager::Instance().Get(packet->m_nPlayerId);

	if (player == nullptr)
	{
		return;
	}

	if (player->m_pEntity->m_pVehicle == nullptr)
	{
		return;
	}
#ifdef _DEV
	CChat::AddMessage("player %d exited from vehicle", packet->m_nPlayerId);
#endif
	if (packet->m_bForce)
	{
		CVector doorPos{};
		player->m_pEntity->m_pVehicle->GetComponentWorldPosition(8, doorPos); // right front door (driver)
		plugin::Command<Commands::WARP_CHAR_FROM_CAR_TO_COORD>(CPools::GetPedRef(player->m_pEntity), doorPos.x, doorPos.y, doorPos.z);
	}
	else
		plugin::Command<Commands::TASK_LEAVE_CAR>(CPools::GetPedRef(player->m_pEntity), CPools::GetVehicleRef(player->m_pEntity->m_pVehicle));
}

// VehicleDamage

void CPacketHandler::VehicleDamage__Handle(void* data, int size)
{
#ifdef _DEV
	CChat::AddMessage("VehicleDamage__Handle");
#endif
	CPackets::VehicleDamage* packet = (CPackets::VehicleDamage*)data;

	CNetworkVehicle* vehicle = CNetworkVehicleManager::Instance().Get(packet->m_nVehicleId);
	*(CDamageManager*)((DWORD)vehicle->m_pEntity + 0x5A0) = packet->m_damageManager;

	DWORD dwVehiclePtr = (DWORD)vehicle->m_pEntity;
	_asm mov ecx, dwVehiclePtr
	_asm mov edx, 0x6B3E90 // CAutomobile::UpdateDamageModel
	_asm call edx
}

// VehicleComponentAdd

void CPacketHandler::VehicleComponentAdd__Handle(void* data, int size)
{
	CPackets::VehicleComponentAdd* packet = (CPackets::VehicleComponentAdd*)data;

	CNetworkVehicle* vehicle = CNetworkVehicleManager::Instance().Get(packet->m_nVehicleId);

	if (vehicle == nullptr)
		return;

	CStreaming::RequestModel(packet->m_nComponentId, eStreamingFlags::GAME_REQUIRED);
	CStreaming::LoadAllRequestedModels(false);
	CStreaming::RequestVehicleUpgrade(packet->m_nComponentId, eStreamingFlags::GAME_REQUIRED);

	char count = 10;
	while (!CStreaming::HasVehicleUpgradeLoaded(packet->m_nComponentId) && count) 
	{
		Sleep(5);
		count--;
	}

	if (!count)
		return;

	vehicle->m_pEntity->AddVehicleUpgrade(packet->m_nComponentId);
}

// VehicleComponentRemove

void CPacketHandler::VehicleComponentRemove__Handle(void* data, int size)
{
	CPackets::VehicleComponentAdd* packet = (CPackets::VehicleComponentAdd*)data;

	CNetworkVehicle* vehicle = CNetworkVehicleManager::Instance().Get(packet->m_nVehicleId);

	if (vehicle == nullptr)
		return;

	vehicle->m_pEntity->RemoveVehicleUpgrade(packet->m_nComponentId);
}

// VehiclePassengerUpdate

CPackets::VehiclePassengerUpdate* CPacketHandler::VehiclePassengerUpdate__Collect(CNetworkVehicle* vehicle, CPlayerPed* localPlayer)
{
	CPackets::VehiclePassengerUpdate* packet = new CPackets::VehiclePassengerUpdate;

	// player data
	packet->m_nAmmoInClip = localPlayer->m_aWeapons[localPlayer->m_nActiveWeaponSlot].m_nAmmoInClip;
	packet->m_nPlayerArmour = (unsigned char)localPlayer->m_fArmour;
	packet->m_nPlayerHealth = (unsigned char)localPlayer->m_fHealth;
	packet->m_nWeapon = localPlayer->m_aWeapons[localPlayer->m_nActiveWeaponSlot].m_eWeaponType;
	packet->m_nVehicleId = vehicle->GetId();
	packet->m_bDriveby = CDriveBy::IsPedInDriveby(localPlayer);
	packet->m_fAimX = *(float*)0xB6F32C + 0x0;
	packet->m_fAimY = *(float*)0xB6F32C + 0x4;

	return packet;
}

void CPacketHandler::VehiclePassengerUpdate__Handle(void* data, int size)
{
	CPackets::VehiclePassengerUpdate* packet = (CPackets::VehiclePassengerUpdate*)data;

	CNetworkVehicle* vehicle = CNetworkVehicleManager::Instance().Get(packet->m_nVehicleId);
	CNetworkPlayer* player = CNetworkPlayerManager::Instance().Get(packet->m_nPlayerId);

	if (vehicle == nullptr || player == nullptr)
		return;

	if (vehicle->m_pEntity == nullptr)
		return;

	if (player->m_pEntity == nullptr)
		return;

	if (!player->m_pEntity->m_nPedFlags.bInVehicle || (player->m_pEntity->m_nPedFlags.bInVehicle && vehicle->m_pEntity->m_pDriver == player->m_pEntity))
	{
#ifdef _DEV
		CChat::AddMessage("forcing enter passenger %d", player->GetId());
#endif
		plugin::Command<Commands::WARP_CHAR_INTO_CAR_AS_PASSENGER>(CPools::GetPedRef(player->m_pEntity), CPools::GetVehicleRef(vehicle->m_pEntity), -1);
	}

	CUtil::GiveWeaponByPacket(player, packet->m_nWeapon, packet->m_nAmmoInClip);

	player->m_pEntity->m_fArmour = player->GetSyncData().m_nArmour = packet->m_nPlayerArmour;
	player->m_pEntity->m_fHealth = player->GetSyncData().m_nHealth = packet->m_nPlayerHealth;
	player->m_aPassengerAim = CVector(packet->m_fAimX, packet->m_fAimY, 0.0f);

	if (packet->m_bDriveby && !CDriveBy::IsPedInDriveby(player->m_pEntity))
	{
		CDriveBy::StartDriveby(player->m_pEntity);
	}
	else if (!packet->m_bDriveby && CDriveBy::IsPedInDriveby(player->m_pEntity))
	{
		CDriveBy::StopDriveby(player->m_pEntity);
	}
}

// PlayerChatMessage

void CPacketHandler::PlayerChatMessage__Handle(void* data, int size)
{
	CPackets::PlayerChatMessage* packet = (CPackets::PlayerChatMessage*)data;
	
	CNetworkPlayer* player = CNetworkPlayerManager::Instance().Get(packet->m_nPlayerId);

	if (player)
	{
		CChat::AddMessage("%s(%d): %s", player->GetName(), player->GetId(), packet->m_message);
	}
}

// PedSpawn

void CPacketHandler::PedSpawn__Handle(void* data, int size)
{
	CPackets::PedSpawn* packet = (CPackets::PedSpawn*)data;

#ifdef _DEV
	CChat::AddMessage("PED SPAWN %d %d %f %f %f %d %d", packet->m_nPedId, packet->m_nModelId, packet->m_vecPosition.x, packet->m_vecPosition.y, packet->m_vecPosition.z, packet->m_nPedType, packet->m_nCreatedBy);
#endif

	CNetworkPed* ped = new CNetworkPed(packet->m_nPedId, (int)packet->m_nModelId, (ePedType)packet->m_nPedType, packet->m_vecPosition, packet->m_nCreatedBy);

	CNetworkPedManager::Instance().Add(ped);
}

// PedRemove

void CPacketHandler::PedRemove__Handle(void* data, int size)
{
	CPackets::PedRemove* packet = (CPackets::PedRemove*)data;

#ifdef _DEV
	CChat::AddMessage("PED REMOVE %d", packet->m_nPedId);
#endif

	CNetworkPed* ped = CNetworkPedManager::Instance().Get(packet->m_nPedId);

	if (ped)
	{
		CNetworkPedManager::Instance().Remove(ped);
		delete ped;
	}
}

// PedOnFoot

CPackets::PedOnFoot* CPacketHandler::PedOnFoot__Collect(CNetworkPed* networkPed)
{
	CPackets::PedOnFoot* packet = new CPackets::PedOnFoot;
	CPed* ped = networkPed->m_pEntity;

	packet->m_nPedId = networkPed->GetId();
	packet->m_syncData.m_vecPosition = ped->m_matrix->pos;
	packet->m_syncData.m_vecVelocity = ped->m_vecMoveSpeed;
	packet->m_syncData.m_nHealth = (unsigned char)ped->m_fHealth;
	packet->m_syncData.m_nArmour = (unsigned char)ped->m_fArmour;
	packet->m_syncData.m_nCurrentWeapon = ped->m_aWeapons[ped->m_nActiveWeaponSlot].m_eWeaponType;
	packet->m_syncData.m_nAmmoInClip = ped->m_aWeapons[ped->m_nActiveWeaponSlot].m_nAmmoInClip;
	packet->m_syncData.m_fAimingRotation = ped->m_fAimingRotation;
	packet->m_syncData.m_fCurrentRotation = ped->m_fCurrentRotation;
	packet->m_syncData.m_fLookDirection = (float)ped->field_73C; // look direction (rad)
	packet->m_syncData.m_nMoveState = (unsigned char)ped->m_nMoveState;

	return packet;
}

void CPacketHandler::PedOnFoot__Handle(void* data, int size)
{
	CPackets::PedOnFoot* packet = (CPackets::PedOnFoot*)data;

	CNetworkPed* ped = CNetworkPedManager::Instance().Get(packet->m_nPedId);

	if (!ped)
		return;

	if (!ped->m_pEntity)
		return;

	if (ped->m_pEntity->m_pVehicle && ped->m_pEntity->m_nPedFlags.bInVehicle)
	{
		//plugin::Command<Commands::TASK_LEAVE_CAR>(CPools::GetPedRef(ped->m_pPed), CPools::GetVehicleRef(ped->m_pPed->m_pEntity));
		plugin::Command<Commands::WARP_CHAR_FROM_CAR_TO_COORD>(CPools::GetPedRef(ped->m_pEntity), packet->m_syncData.m_vecPosition.x, packet->m_syncData.m_vecPosition.y, packet->m_syncData.m_vecPosition.z);
	}

	CUtil::GiveWeaponByPacket(ped, packet->m_syncData.m_nCurrentWeapon, packet->m_syncData.m_nAmmoInClip);

	auto& syncData = ped->GetSyncData();

	ped->m_pEntity->m_matrix->pos = packet->m_syncData.m_vecPosition;
	syncData.m_fCurrentRotation = ped->m_pEntity->m_fCurrentRotation = packet->m_syncData.m_fCurrentRotation;
	syncData.m_fAimingRotation = ped->m_pEntity->m_fAimingRotation = packet->m_syncData.m_fAimingRotation;
	syncData.m_fLookDirection = packet->m_syncData.m_fLookDirection;
	ped->m_pEntity->m_fHealth = packet->m_syncData.m_nHealth;
	ped->m_pEntity->m_fArmour = packet->m_syncData.m_nArmour;
	syncData.m_vecVelocity = packet->m_syncData.m_vecVelocity;
	syncData.m_nMoveState = (eMoveState)packet->m_syncData.m_nMoveState;
}

// GameWeatherTime

CPackets::GameWeatherTime* CPacketHandler::GameWeatherTime__Collect()
{
	CPackets::GameWeatherTime* packet = new CPackets::GameWeatherTime;
	packet->m_nNewWeather = (uint8_t)CWeather::NewWeatherType;
	packet->m_nOldWeather = (uint8_t)CWeather::OldWeatherType;
	packet->m_nForcedWeather = (uint8_t)CWeather::ForcedWeatherType;
	packet->m_nCurrentMonth = CClock::ms_nGameClockMonth;
	packet->m_nCurrentDay = CClock::CurrentDay;
	packet->m_nCurrentHour = CClock::ms_nGameClockHours;
	packet->m_nCurrentMinute = CClock::ms_nGameClockMinutes;
	packet->m_nGameTickCount = CClock::ms_nMillisecondsPerGameMinute;
	return packet;
}

void CPacketHandler::GameWeatherTime__Handle(void* data, int size)
{
	CPackets::GameWeatherTime* packet = (CPackets::GameWeatherTime*)data;

	CWeather::NewWeatherType = packet->m_nNewWeather;
	CWeather::OldWeatherType = packet->m_nOldWeather;
	CWeather::ForcedWeatherType = packet->m_nForcedWeather;
	CClock::ms_nGameClockMonth = packet->m_nCurrentMonth;
	CClock::CurrentDay = packet->m_nCurrentDay;
	CClock::ms_nGameClockHours = packet->m_nCurrentHour;
	CClock::ms_nGameClockMinutes = packet->m_nCurrentMinute;
	CClock::ms_nMillisecondsPerGameMinute = packet->m_nGameTickCount;
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

	CNetworkPlayer* player = CNetworkPlayerManager::Instance().Get(packet->m_nPlayerId);
	player->m_compressedControllerState = packet->m_controllerState;
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
	packet->m_nPedId = ped->GetId();
	packet->m_nAmmoInClip = ped->m_pEntity->m_aWeapons[ped->m_pEntity->m_nActiveWeaponSlot].m_nAmmoInClip;
	packet->m_nPedArmour = (unsigned char)ped->m_pEntity->m_fArmour;
	packet->m_nPedHealth = (unsigned char)ped->m_pEntity->m_fHealth;
	packet->m_nWeapon = ped->m_pEntity->m_aWeapons[ped->m_pEntity->m_nActiveWeaponSlot].m_eWeaponType;

	// vehicle data
	packet->m_nVehicleId = vehicle->GetId();
	packet->m_syncData.m_vecPosition = vehicle->m_pEntity->m_matrix->pos;
	packet->m_syncData.m_vecRoll = vehicle->m_pEntity->m_matrix->right;
	packet->m_syncData.m_vecRotation = vehicle->m_pEntity->m_matrix->up;
	packet->m_syncData.m_vecVelocity = vehicle->m_pEntity->m_vecMoveSpeed;

	packet->m_syncData.m_nPrimaryColor = vehicle->m_pEntity->m_nPrimaryColor;
	packet->m_syncData.m_nSecondaryColor = vehicle->m_pEntity->m_nSecondaryColor;

	packet->m_syncData.m_nHealth = vehicle->m_pEntity->m_fHealth;

	packet->m_syncData.m_nPaintjob = vehicle->m_nPaintJob;

	eVehicleType vehicleType = CUtil::GetVehicleType(vehicle->m_pEntity);
	if (vehicleType == eVehicleType::VEHICLE_BIKE || vehicleType == eVehicleType::VEHICLE_BMX)
	{
		CBike* bike = (CBike*)vehicle->m_pEntity;

		packet->m_syncData.m_fBikeLean = bike->m_rideAnimData.m_fAnimLean;
	}

	if (vehicleType == eVehicleType::VEHICLE_PLANE)
	{
		CPlane* plane = (CPlane*)vehicle->m_pEntity;
		packet->m_syncData.m_fPlaneGearState = plane->m_fLandingGearStatus;
	}

	packet->m_syncData.m_nDoorLock = vehicle->m_pEntity->m_eDoorLock;
	
	packet->m_autoPilot = CSyncAutoPilot(vehicle->m_pEntity->m_autoPilot);

	packet->m_fGasPedal = vehicle->m_pEntity->m_fGasPedal;
	packet->m_fBreakPedal = vehicle->m_pEntity->m_fBreakPedal;
	packet->m_fSteerAngle = vehicle->m_pEntity->m_fSteerAngle;

	return packet;
}

void CPacketHandler::PedDriverUpdate__Handle(void* data, int size)
{
	CPackets::PedDriverUpdate* packet = (CPackets::PedDriverUpdate*)data;

	CNetworkVehicle* vehicle = CNetworkVehicleManager::Instance().Get(packet->m_nVehicleId);
	CNetworkPed* ped = CNetworkPedManager::Instance().Get(packet->m_nPedId);

	if (vehicle == nullptr || ped == nullptr || ped->m_pEntity == nullptr)
		return;

	if (vehicle->m_pEntity == nullptr)
	{
		//vehicle->CreateVehicle(vehicle->m_nVehicleId, vehicle->m_nModelId, packet->pos, 0.f, packet->color1, packet->color2);
		return;
	}

	if (ped->m_pEntity->m_pVehicle != vehicle->m_pEntity)
		plugin::Command<Commands::WARP_CHAR_INTO_CAR>(CPools::GetPedRef(ped->m_pEntity), CPools::GetVehicleRef(vehicle->m_pEntity));

	if(CUtil::IsPositionUpdateNeeded(packet->m_syncData.m_vecPosition, vehicle->m_pEntity->m_matrix->pos))
		vehicle->m_pEntity->m_matrix->pos = packet->m_syncData.m_vecPosition;

	vehicle->m_pEntity->m_matrix->right = packet->m_syncData.m_vecRoll;
	vehicle->m_pEntity->m_matrix->up = packet->m_syncData.m_vecRotation;
	ped->GetSyncData().m_vecVelocity = packet->m_syncData.m_vecVelocity;
	vehicle->m_pEntity->m_vecMoveSpeed = packet->m_syncData.m_vecVelocity;

	CUtil::GiveWeaponByPacket(ped, packet->m_nWeapon, packet->m_nAmmoInClip);

	ped->m_pEntity->m_fArmour = packet->m_nPedArmour;
	ped->m_pEntity->m_fHealth = packet->m_nPedHealth;

	vehicle->m_pEntity->m_nPrimaryColor = packet->m_syncData.m_nPrimaryColor;
	vehicle->m_pEntity->m_nSecondaryColor = packet->m_syncData.m_nSecondaryColor;

	vehicle->m_pEntity->m_fHealth = packet->m_syncData.m_nHealth;

	if (vehicle->m_nPaintJob != packet->m_syncData.m_nPaintjob)
		vehicle->m_pEntity->SetRemap(packet->m_syncData.m_nPaintjob);

	eVehicleType vehicleType = CUtil::GetVehicleType(vehicle->m_pEntity);
	if (vehicleType == eVehicleType::VEHICLE_BIKE || vehicleType == eVehicleType::VEHICLE_BMX)
	{
		CBike* bike = (CBike*)vehicle->m_pEntity;

		*(float*)((DWORD)&*bike + 0x64C) = packet->m_syncData.m_fBikeLean;
	}

	if (CUtil::GetVehicleType(vehicle->m_pEntity) == eVehicleType::VEHICLE_PLANE)
	{
		CPlane* plane = (CPlane*)vehicle->m_pEntity;

		plane->m_fLandingGearStatus = packet->m_syncData.m_fPlaneGearState;
	}

	vehicle->m_pEntity->m_eDoorLock = (eDoorLock)packet->m_syncData.m_nDoorLock;
	packet->m_autoPilot.WriteTo(vehicle->m_pEntity->m_autoPilot);
	ped->m_autoPilot = vehicle->m_pEntity->m_autoPilot;

	ped->m_fGasPedal = packet->m_fGasPedal;
	ped->m_fBreakPedal = packet->m_fBreakPedal;
	ped->m_fSteerAngle = packet->m_fSteerAngle;
}

// EntityStream
// TODO:
//void CPacketHandler::EntityStream__Handle(void* data, int size)
//{
//	CPackets::EntityStream* packet = (CPackets::EntityStream*)data;
//	
//	switch (packet->m_nEntityType)
//	{
//	case eNetworkEntityType::NETWORK_ENTITY_PLAYER:
//	{
//		if (auto player = CNetworkPlayerManager::Instance().Get(packet->m_nNetworkId))
//		{
//			if (packet->m_bIn)
//				player->StreamIn(player->m_lOnFoot->position);
//			else
//				player->StreamOut();
//		}
//
//		break;
//	}
//	case eNetworkEntityType::NETWORK_ENTITY_VEHICLE:
//	case eNetworkEntityType::NETWORK_ENTITY_PED:
//		break;
//	}
//}