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
			// preload model
			CStreaming::RequestModel(CUtil::GetWeaponModelById(packet->weapon), eStreamingFlags::GAME_REQUIRED | eStreamingFlags::PRIORITY_REQUEST);
			CStreaming::LoadAllRequestedModels(false);

			// give weapon
			if (activeWeapon.m_nTotalAmmo <= 500)
			{
				player->m_pPed->GiveWeapon((eWeaponType)packet->weapon, 99999, false);
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


	// save last onfoot sync
	player->m_oOnFoot = player->m_lOnFoot;
	player->m_lOnFoot = packet;
}