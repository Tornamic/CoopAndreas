#pragma once

#include "stdafx.h"


enum CPacketsID : unsigned short
{
	PLAYER_CONNECTED,
	PLAYER_DISCONNECTED,
	PLAYER_ONFOOT,
	PLAYER_BULLET_SHOT,
	PLAYER_HANDSHAKE,
	PLAYER_PLACE_WAYPOINT,
	PLAYER_GET_NAME,
	VEHICLE_SPAWN,
	PLAYER_SET_HOST,
	ADD_EXPLOSION,
	VEHICLE_REMOVE
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
		float aimX;
		float aimY;


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

	struct PlayerPlaceWaypoint
	{
		int playerid;
		bool place;
		CVector position;
	};

	struct PlayerGetName
	{
		int playerid;
		char name[32 + 1];
	};

	struct PlayerSetHost
	{
		int playerid;
	};

	struct AddExplosion
	{
		unsigned char type;
		CVector pos;
		int time;
		char usesSound;
		float cameraShake;
		char isVisible;
	};

	struct VehicleSpawn
	{
		int vehicleid;
		unsigned short modelid;
		CVector pos;
		float rot;
	};

	struct VehicleRemove
	{
		int vehicleid;
	};
};