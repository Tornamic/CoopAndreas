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

		// last sent
		static inline PlayerOnFoot* m_last = nullptr;
	};
};