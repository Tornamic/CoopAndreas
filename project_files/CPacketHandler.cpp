#include "stdafx.h"

// PlayerConnected

void CPacketHandler::PlayerConnected__Handle(void* data, int size)
{
	// get packet struct
	CPackets::PlayerConnected* packet = (CPackets::PlayerConnected*)data;

	// create new player
	CNetworkPlayer* player = new CNetworkPlayer(packet->id, CVector(2246.506f, -1259.552f, 23.9531f));

	// add player to list
	CNetworkPlayerManager::Add(player);

	CChat::AddMessage("[Player] " + std::string(player->GetName()) + " connected");
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

	// get player key state, not all keyboard, just controller keys
	packet->controllerState = player->GetPadFromPlayer()->NewState;
	
	// get player health, armour
	packet->health = (unsigned char)player->m_fHealth;
	packet->armour = (unsigned char)player->m_fArmour;

	// get player weapon in hands
	packet->weapon = player->m_aWeapons[player->m_nActiveWeaponSlot].m_eWeaponType;

	// get ammo in clip count
	packet->ammo = player->m_aWeapons[player->m_nActiveWeaponSlot].m_nAmmoInClip;

	// get crouch state
	packet->ducking = CUtil::IsDucked(player);

	// get player camera aim
	packet->aimX = CLocalPlayer::m_vecLastAimX;
	packet->aimY = CLocalPlayer::m_vecLastAimY;

	return packet;
}

void CPacketHandler::PlayerOnFoot__Handle(void* data, int size)
{
	// get packet struct
	CPackets::PlayerOnFoot* packet = (CPackets::PlayerOnFoot*)data;

	// get player instance 
	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->id);

	// check if player not connected
	if (player == nullptr)
		return;

	if (CUtil::IsPositionUpdateNeeded(player->m_pPed->m_matrix->pos, packet->position))
	{
		player->m_pPed->m_matrix->pos = packet->position;
	}

	CUtil::GiveWeaponByPacket(player, packet->weapon, packet->ammo);

	player->m_pPed->m_fAimingRotation =
		player->m_pPed->m_fCurrentRotation = packet->rotation;

	// save last onfoot sync
	player->m_oOnFoot = player->m_lOnFoot;
	player->m_lOnFoot = packet;
	
}

// PlayerBulletShot

void CPacketHandler::PlayerBulletShot__Handle(void* data, int size)
{
	CPackets::PlayerBulletShot* packet = (CPackets::PlayerBulletShot*)data;

	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->playerid);

	CEntity* victim = NULL;
	
	if (packet->targetid != -1)
	{

		CNetworkPlayer* damagedPlayer = CNetworkPlayerManager::GetPlayer(packet->targetid);
		CNetworkVehicle* damagedVehicle = CNetworkVehicleManager::GetVehicle(packet->targetid);

		if (damagedPlayer != nullptr)
			victim = damagedPlayer->m_pPed;

		if (damagedVehicle != nullptr)
			victim = damagedVehicle->m_pVehicle;

		if (packet->targetid == CNetworkPlayerManager::m_nMyId && packet->entityType == eEntityType::ENTITY_TYPE_PED)
			victim = FindPlayerPed(0);
	}

	player->m_pPed->m_aWeapons[player->m_pPed->m_nActiveWeaponSlot].DoBulletImpact(player->m_pPed, victim, &packet->startPos, &packet->endPos, &packet->colPoint, packet->incrementalHit);
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

	printf("%d %f %f\n", packet->place, packet->position.x, packet->position.y);
}

// PlayerGetName

void CPacketHandler::PlayerGetName__Handle(void* data, int size)
{
	CPackets::PlayerGetName* packet = (CPackets::PlayerGetName*)data;
	
	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->playerid);

	strcpy_s(player->m_Name, packet->name);

	CChat::AddMessage("[Player] player " + std::to_string(player->m_iPlayerId) + " now aka " + player->m_Name);
}

// PlayerSetHost

void CPacketHandler::PlayerSetHost__Handle(void* data, int size)
{
	CPackets::PlayerSetHost* packet = (CPackets::PlayerSetHost*)data;

	if (packet->playerid == CNetworkPlayerManager::m_nMyId)
	{
		CLocalPlayer::m_bIsHost = true;
		CChat::AddMessage("[Player] You are the host now");
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
	plugin::Call<0x736A50, CEntity*, CPed*, int, CVector2D, float, int, char, float, char>(nullptr, nullptr, 
		packet->type, 
		CVector2D(packet->pos.x, packet->pos.y), 
		packet->pos.z, 
		packet->time, 
		packet->usesSound, 
		packet->cameraShake, 
		packet->isVisible);
}

// VehicleSpawn

void CPacketHandler::VehicleSpawn__Handle(void* data, int size)
{
	CPackets::VehicleSpawn* packet = (CPackets::VehicleSpawn*)data;
	
	char buffer[128];
	sprintf(buffer, "cum spawn %d %d %f %f %f %f", packet->vehicleid, packet->modelid, packet->pos.x, packet->pos.y, packet->pos.z, packet->rot);
	CChat::AddMessage(buffer);

	CNetworkVehicle* vehicle = new CNetworkVehicle
	(
		packet->vehicleid,
		packet->modelid,
		packet->pos,
		packet->rot,
		packet->color1,
		packet->color2
	);

	CNetworkVehicleManager::Add(vehicle);
}

// VehicleRemove

void CPacketHandler::VehicleRemove__Handle(void* data, int size)
{
	CPackets::VehicleRemove* packet = (CPackets::VehicleRemove*)data;

	char buffer[128];
	sprintf(buffer, "cum remove %d", packet->vehicleid);
	CChat::AddMessage(buffer);

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
	packet->color1 = vehicle->m_pVehicle->m_nPrimaryColor;
	packet->color2 = vehicle->m_pVehicle->m_nSecondaryColor;
	packet->health = vehicle->m_pVehicle->m_fHealth;
	return packet;
}

void CPacketHandler::VehicleIdleUpdate__Handle(void* data, int size)
{
	CPackets::VehicleIdleUpdate* packet = (CPackets::VehicleIdleUpdate*)data;
	CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(packet->vehicleid);

	if (vehicle == nullptr)
		return;

	if (vehicle->m_pVehicle == nullptr)
		vehicle->CreateVehicle(vehicle->m_nVehicleId, vehicle->m_nModelId, packet->pos, 0.f, packet->color1, packet->color2);

	vehicle->m_pVehicle->m_matrix->pos = packet->pos;		   
	vehicle->m_pVehicle->m_matrix->right = packet->roll;	   
	vehicle->m_pVehicle->m_matrix->up = packet->rot;		   
	vehicle->m_pVehicle->m_vecMoveSpeed = packet->velocity;

	vehicle->m_pVehicle->m_nPrimaryColor = packet->color1;
	vehicle->m_pVehicle->m_nSecondaryColor = packet->color2;
	vehicle->m_pVehicle->m_fHealth = packet->health;
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
	packet->controllerState =	player->GetPadFromPlayer()->NewState;
	packet->playerArmour =		(unsigned char)player->m_fArmour;
	packet->playerHealth =		(unsigned char)player->m_fHealth;
	packet->weapon =			player->m_aWeapons[player->m_nActiveWeaponSlot].m_eWeaponType;

	packet->color1 = vehicle->m_pVehicle->m_nPrimaryColor;
	packet->color2 = vehicle->m_pVehicle->m_nSecondaryColor;

	packet->health = vehicle->m_pVehicle->m_fHealth;

	return packet;
}

void CPacketHandler::VehicleDriverUpdate__Handle(void* data, int size)
{
	CPackets::VehicleDriverUpdate* packet = (CPackets::VehicleDriverUpdate*)data;

	CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(packet->vehicleid);
	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->playerid);

	if (vehicle == nullptr || player == nullptr)
		return;

	if (vehicle->m_pVehicle == nullptr)
	{
		vehicle->CreateVehicle(vehicle->m_nVehicleId, vehicle->m_nModelId, packet->pos, 0.f, packet->color1, packet->color2);
		return;
	}

	if (player->m_pPed == nullptr)
	{
		player->CreatePed(player->m_iPlayerId, packet->pos);
		return;
	}

	if (player->m_pPed->m_pVehicle != vehicle->m_pVehicle)
		plugin::Command<Commands::WARP_CHAR_INTO_CAR>(CPools::GetPedRef(player->m_pPed), CPools::GetVehicleRef(vehicle->m_pVehicle));

	vehicle->m_pVehicle->m_matrix->pos = packet->pos;
	vehicle->m_pVehicle->m_matrix->right = packet->roll;
	vehicle->m_pVehicle->m_matrix->up = packet->rot;
	vehicle->m_pVehicle->m_vecMoveSpeed = packet->velocity;
	
	CUtil::GiveWeaponByPacket(player, packet->weapon, packet->ammo);

	player->m_oOnFoot = player->m_lOnFoot;
	
	player->m_lOnFoot->controllerState = packet->controllerState;
	player->m_lOnFoot->armour = packet->playerArmour;
	player->m_lOnFoot->health = packet->playerHealth;

	vehicle->m_pVehicle->m_nPrimaryColor = packet->color1;
	vehicle->m_pVehicle->m_nSecondaryColor = packet->color2;

	vehicle->m_pVehicle->m_fHealth = packet->health;
}

// VehicleEnter

void CPacketHandler::VehicleEnter__Handle(void* data, int size)
{
	CPackets::VehicleEnter* packet = (CPackets::VehicleEnter*)data;
	
	CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(packet->playerid);
	CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(packet->vehicleid);

	if (player == nullptr || vehicle == nullptr)
	{
		return;
	}


	CChat::AddMessage("player %d entered vehicleid %d", packet->playerid, packet->vehicleid);

	if (packet->seatid == 0) // driver
	{
		if (packet->force)
		{
			for (unsigned char i = 0; i < 5; i++)
			{
				if (vehicle->m_pVehicle)
					break;

				Sleep(100);
			}

			plugin::Command<Commands::WARP_CHAR_INTO_CAR>(CPools::GetPedRef(player->m_pPed), CPools::GetVehicleRef(vehicle->m_pVehicle));
		}
		else
			plugin::Command<Commands::TASK_ENTER_CAR_AS_DRIVER>(CPools::GetPedRef(player->m_pPed), CPools::GetVehicleRef(vehicle->m_pVehicle), 3000);
	}
	else // passenger (todo)
	{

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

	CChat::AddMessage("player %d exited from vehicle", packet->playerid);

	if (packet->force)
	{
		CVector doorPos{};
		player->m_pPed->m_pVehicle->GetComponentWorldPosition(8, doorPos); // right front door (driver)
		plugin::Command<Commands::WARP_CHAR_FROM_CAR_TO_COORD>(CPools::GetPedRef(player->m_pPed), doorPos.x, doorPos.y, doorPos.z);
	}
	else
		plugin::Command<Commands::TASK_LEAVE_CAR>(CPools::GetPedRef(player->m_pPed), CPools::GetVehicleRef(player->m_pPed->m_pVehicle));
}

// VehicleDamage

void CPacketHandler::VehicleDamage__Handle(void* data, int size)
{
	CChat::AddMessage("VehicleDamage__Handle");

	CPackets::VehicleDamage* packet = (CPackets::VehicleDamage*)data;

	CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(packet->vehicleid);
	*(CDamageManager*)((DWORD)vehicle->m_pVehicle + 0x5A0) = packet->damageManager;

	DWORD dwVehiclePtr = (DWORD)vehicle->m_pVehicle;
	_asm mov ecx, dwVehiclePtr
	_asm mov edx, 0x6B3E90 // CAutomobile::UpdateDamageModel
	_asm call edx
}
