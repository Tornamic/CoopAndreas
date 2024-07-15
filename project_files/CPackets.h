#pragma once

#include "stdafx.h"


enum CPacketsID : unsigned short
{
	PLAYER_CONNECTED,
	PLAYER_DISCONNECTED,
	PLAYER_ONFOOT
};

class CPackets
{
public:
	struct PlayerConnected
	{
		int id;
		static void Handle(void* data, int size)
		{
			// get packet struct
			CPackets::PlayerConnected* packet = (CPackets::PlayerConnected*)data;

			// create new player
			CNetworkPlayer* player = new CNetworkPlayer(packet->id, CVector(2246.506f, -1259.552f, 23.9531f));

			// add player to list
			CNetworkPlayerManager::Add(player);
		}
	};

	struct PlayerDisconnected
	{
		int id;
		unsigned char reason;

		static void Handle(void* data, int size)
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
	};

	struct PlayerOnFoot
	{
		int id;
		CVector position;
		CVector velocity;
		float rotation;
		CControllerState controllerState;

		// last sent
		static inline PlayerOnFoot* m_last = nullptr;

		static PlayerOnFoot* Collect()
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

			return packet;
		}

		static void Handle(void* data, int size)
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

			// save last onfoot sync
			player->m_oOnFoot = player->m_lOnFoot;
			player->m_lOnFoot = packet;
		}
	};
};