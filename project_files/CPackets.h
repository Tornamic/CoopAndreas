#pragma once

#include "CUtil.h"

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
	};

	struct PlayerDisconnected
	{
		int id;
		unsigned char reason;
	};

	struct PlayerOnFoot
	{
		int id;
		CVector position;
		CVector velocity;
		float rotation;
		CControllerState controllerState;
		bool isDucked;

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

			// does player ducked
			packet->isDucked = CUtil::IsDucked(player);

			return packet;
		}
	};
};