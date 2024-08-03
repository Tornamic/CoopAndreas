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
	packet->health = player->m_fHealth;
	packet->armour = player->m_fArmour;

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

	// check if player not created
	if (player == nullptr)
		return;

	if (CUtil::IsPositionUpdateNeeded(player->m_pPed->m_matrix->pos, packet->position))
	{
		player->m_pPed->m_matrix->pos = packet->position;
	}

	// update weapon, ammo
	auto& activeWeapon = player->m_pPed->m_aWeapons[player->m_pPed->m_nActiveWeaponSlot];
	bool isWeaponTypeDifferent = (activeWeapon.m_eWeaponType != packet->weapon);
	bool isAmmoDifferent = (activeWeapon.m_nAmmoInClip != packet->ammo);

	if (isWeaponTypeDifferent || isAmmoDifferent)
	{
		CWorld::PlayerInFocus = player->GetInternalId();

		if (isWeaponTypeDifferent)
		{
			player->m_pPed->ClearWeapons();
		}

		if (packet->weapon != 0)
		{
			if (isWeaponTypeDifferent)
			{
				// preload model
				CStreaming::RequestModel(CUtil::GetWeaponModelById(packet->weapon), eStreamingFlags::GAME_REQUIRED | eStreamingFlags::PRIORITY_REQUEST);
				CStreaming::LoadAllRequestedModels(false);
			}

			// give weapon
			bool isMeleeWeapon = CUtil::IsMeleeWeapon(packet->weapon);

			if (activeWeapon.m_nTotalAmmo <= 0)
			{
				player->m_pPed->GiveWeapon((eWeaponType)packet->weapon, isMeleeWeapon ? 1 : 99999, false);
			}


			// set ammo in clip
			activeWeapon.m_nAmmoInClip = packet->ammo;
		}

		if (isWeaponTypeDifferent)
		{
			player->m_pPed->SetCurrentWeapon((eWeaponType)packet->weapon);
		}

		CWorld::PlayerInFocus = 0;
	}

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

		if(damagedPlayer != nullptr)
			victim = damagedPlayer->m_pPed;

		if (packet->targetid == CNetworkPlayerManager::m_nMyId)
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

	printf("got name %s\n", player->m_Name);
}