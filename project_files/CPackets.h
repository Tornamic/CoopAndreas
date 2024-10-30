#pragma once

#include "stdafx.h"
#include "eNetworkEntityType.h"


enum ePacketType : unsigned short
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
	PED_ONFOOT,
	GAME_WEATHER_TIME,
	PED_ADD_TASK,
	PED_REMOVE_TASK,
	PLAYER_KEY_SYNC,
	PED_DRIVER_UPDATE,
	ENTITY_STREAM
};

class CPackets
{
public:

	struct PlayerConnected
	{
		uint16_t id;
	};

	struct PlayerDisconnected
	{
		uint16_t id;
		unsigned char reason;
	};

	struct PlayerOnFoot
	{
		uint16_t id = 0;
		CVector position = CVector();
		CVector velocity = CVector();
		float rotation = 0.0f;
		unsigned char health = 100;
		unsigned char armour = 0;
		unsigned char weapon = 0;
		unsigned short ammo = 0;
		bool ducking = false;
		float aimX = 0.0f;
		float aimY = 0.0f;
		bool hasJetpack = false;
	};

	#pragma pack(1)
	struct PlayerBulletShot
	{
		uint16_t playerid;
		uint16_t targetid;
		CVector startPos;
		CVector endPos;
		CColPoint colPoint;
		int incrementalHit;
		unsigned char entityType;
	};

	struct PlayerHandshake
	{
		uint16_t yourid;
	};

	struct PlayerPlaceWaypoint
	{
		uint16_t playerid;
		bool place;
		CVector position;
	};

	struct PlayerGetName
	{
		uint16_t playerid;
		char name[32 + 1];
	};

	struct PlayerSetHost
	{
		uint16_t playerid;
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
		uint16_t vehicleid;
		unsigned short modelid;
		CVector pos;
		float rot;
		unsigned char color1;
		unsigned char color2;
	};

	struct VehicleRemove
	{
		uint16_t vehicleid;
	};

	struct VehicleIdleUpdate
	{
		uint16_t vehicleid;
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
		uint16_t playerid;
		uint16_t vehicleid;
		CVector pos;
		CVector rot;
		CVector roll;
		CVector velocity;
		unsigned char playerHealth;
		unsigned char playerArmour;
		unsigned char weapon;
		unsigned short ammo;
		unsigned char color1;
		unsigned char color2;
		float health;
		char paintjob;
		float bikeLean;
		float turretAimHorizontal;
		float turretAimVertical;
		unsigned short miscComponentAngle; // hydra thrusters
		float planeGearState;
		unsigned char locked;
	};

	struct VehicleEnter
	{
		uint16_t playerid;
		uint16_t vehicleid;
		unsigned char seatid;
		bool force;
	};

	struct VehicleExit
	{
		uint16_t playerid;
		bool force;
	};

	struct VehicleDamage
	{
		uint16_t vehicleid;
		CDamageManager damageManager;
	};

	struct VehicleComponentAdd
	{
		uint16_t vehicleid;
		int componentid;
	};

	struct VehicleComponentRemove
	{
		uint16_t vehicleid;
		int componentid;
	};

	struct VehiclePassengerUpdate
	{
		uint16_t playerid;
		uint16_t vehicleid;
		unsigned char playerHealth;
		unsigned char playerArmour;
		unsigned char weapon;
		unsigned short ammo;
		unsigned char driveby;
		CVector aim;
	};

	struct PlayerChatMessage
	{
		uint16_t playerid;
		char message[128+1];
	};

	struct PedSpawn
	{
		uint16_t pedid;
		short modelId;
		unsigned char pedType;
		CVector pos;
		unsigned char createdBy;
	};

	struct PedRemove
	{
		uint16_t pedid;
	};

	struct PedOnFoot
	{
		uint16_t pedid = 0;
		CVector pos = CVector();
		CVector velocity = CVector();
		unsigned char health = 100;
		unsigned char armour = 0;
		unsigned char weapon = 0;
		unsigned short ammo = 0;
		float aimingRotation = 0.0f;
		float currentRotation = 0.0f;
		float lookDirection = 0.0f;
		unsigned char moveState = 0;
	};
	
	struct GameWeatherTime
	{
		unsigned char newWeather;
		unsigned char oldWeather;
		unsigned char forcedWeather;
		unsigned char currentMonth;
		unsigned char currentDay;
		unsigned char currentHour;
		unsigned char currentMinute;
		unsigned int gameTickCount;
	};

	struct PedRemoveTask
	{
		uint16_t pedid;
		eTaskType taskid;
	};

	struct PlayerKeySync
	{
		uint16_t playerid;
		ÑCompressedControllerState newState;
	};

	struct PedDriverUpdate
	{
		uint16_t pedid;
		uint16_t vehicleid;
		CVector pos;
		CVector rot;
		CVector roll;
		CVector velocity;
		unsigned char pedHealth;
		unsigned char pedArmour;
		unsigned char weapon;
		unsigned short ammo;
		unsigned char color1;
		unsigned char color2;
		float health;
		char paintjob;
		float bikeLean;
		float planeGearState;
		unsigned char locked;
		CSyncAutoPilot autoPilot;
		float gasPedal;
		float breakPedal;
		float steerAngle;
	};

	struct EntityStream
	{
		uint16_t m_nNetworkId;
		union
		{
			struct
			{
				eNetworkEntityType m_nEntityType : 3;
				unsigned char m_bIn : 1; // in == true, out == false
			};
		};
	};
};