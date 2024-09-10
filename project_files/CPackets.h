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
	VEHICLE_REMOVE,
	VEHICLE_IDLE_UPDATE,
	VEHICLE_DRIVER_UPDATE,
	VEHICLE_ENTER,
	VEHICLE_EXIT,
	VEHICLE_DAMAGE,
	VEHICLE_COMPONENT_ADD,
	VEHICLE_COMPONENT_REMOVE,
	VEHICLE_PASSENGER_UPDATE,
	PLAYER_CHAT_MESSAGE,
	PED_SPAWN,
	PED_REMOVE,
	PED_ONFOOT
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
		int id = 0;
		CVector position = CVector();
		CVector velocity = CVector();
		float rotation = 0.0f;
		CControllerState controllerState = CControllerState();
		unsigned char health = 100;
		unsigned char armour = 0;
		unsigned char weapon = 0;
		unsigned short ammo = 0;
		bool ducking = false;
		float aimX = 0.0f;
		float aimY = 0.0f;
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
		unsigned char entityType;
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
		unsigned char color1;
		unsigned char color2;
	};

	struct VehicleRemove
	{
		int vehicleid;
	};

	struct VehicleIdleUpdate
	{
		int vehicleid;
		CVector pos;
		CVector rot;
		CVector roll;
		CVector velocity;
		unsigned char color1;
		unsigned char color2;
		float health;
		char paintjob;
		float planeGearState;
		unsigned char locked;
	};

	struct VehicleDriverUpdate
	{
		int playerid;
		int vehicleid;
		CVector pos;
		CVector rot;
		CVector roll;
		CVector velocity;
		CControllerState controllerState;
		unsigned char playerHealth;
		unsigned char playerArmour;
		unsigned char weapon;
		unsigned short ammo;
		unsigned char color1;
		unsigned char color2;
		float health;
		char paintjob;
		float turretAimHorizontal;
		float turretAimVertical;
		unsigned short miscComponentAngle; // hydra thrusters
		float planeGearState;
		unsigned char locked;
	};

	struct VehicleEnter
	{
		int playerid;
		int vehicleid;
		unsigned char seatid;
		bool force;
	};

	struct VehicleExit
	{
		int playerid;
		bool force;
	};

	struct VehicleDamage
	{
		int vehicleid;
		CDamageManager damageManager;
	};

	struct VehicleComponentAdd
	{
		int vehicleid;
		int componentid;
	};

	struct VehicleComponentRemove
	{
		int vehicleid;
		int componentid;
	};

	struct VehiclePassengerUpdate
	{
		int playerid;
		int vehicleid;
		CControllerState controllerState;
		unsigned char playerHealth;
		unsigned char playerArmour;
		unsigned char weapon;
		unsigned short ammo;
		unsigned char driveby;
		CVector aim;
	};

	struct PlayerChatMessage
	{
		int playerid;
		char message[128+1];
	};

	struct PedSpawn
	{
		int pedid;
		short modelId;
		unsigned char pedType;
		CVector pos;
		unsigned char createdBy;
	};

	struct PedRemove
	{
		int pedid;
	};

	struct PedOnFoot
	{
		int pedid = 0;
		CVector pos = CVector();
		CVector velocity = CVector();
		float rot = 0.f;
		unsigned char health = 100;
		unsigned char armour = 0;
		unsigned char weapon = 0;
		unsigned short ammo = 0;
		// todo action sync
		// todo path sync
		// todo tasks sync (event-based)
	};
};