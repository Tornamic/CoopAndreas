#pragma once

#include "stdafx.h"


enum CPacketsID : unsigned short
{
	PLAYER_CONNECTED,
	PLAYER_DISCONNECTED,
	PLAYER_ONFOOT,
	PLAYER_BULLET_SHOT,
	PLAYER_HANDSHAKE
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
		unsigned char health;
		unsigned char armour;
		unsigned char weapon;
		unsigned short ammo;
		bool ducking;

		CAMERA_AIM aim;
		unsigned char cameraMode;

		// last sent
		static inline PlayerOnFoot* m_last = nullptr;
	};

	#pragma pack(1)
	struct PlayerBulletShot
	{
		int playerid;
		int targetid;
		CVector startPos;
		CVector endPos;
		CColPoint colPoint;
		int incrementalHit;
	};

	struct PlayerHandshake
	{
		int yourid;
	};
};