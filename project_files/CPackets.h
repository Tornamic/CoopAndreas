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
		uint16_t m_nPlayerId;
	};

	struct PlayerDisconnected
	{
		uint16_t m_nPlayerId;
		uint8_t m_nReason;
	};

	struct PlayerOnFoot
	{
		uint16_t m_nPlayerId;
		PlayerSyncData m_syncData;
	};

	#pragma pack(1)
	struct PlayerBulletShot
	{
		uint16_t m_nPlayerId;
		uint16_t m_nTargetId;
		CVector m_vecStartPos;
		CVector m_vecEndPos;
		CColPoint m_colPoint;
		int32_t m_nIncrementalHit;
		uint8_t m_nTargetEntityType;
	};

	struct PlayerHandshake
	{
		uint16_t m_nPlayerId;
	};

	struct PlayerPlaceWaypoint
	{
		uint16_t m_nPlayerId;
		bool m_bPlace;
		CVector m_vecPosition;
	};

	struct PlayerGetName
	{
		uint16_t m_nPlayerId;
		char m_aName[32 + 1];
	};

	struct PlayerSetHost
	{
		uint16_t m_nPlayerId;
	};

	struct AddExplosion
	{
		uint8_t m_nType;
		CVector m_vecPosition;
		int32_t m_nTime;
		bool m_bUsesSound;
		float m_fCameraShake;
		bool m_bVisible;
	};

	struct VehicleSpawn
	{
		uint16_t m_nVehicleId;
		int16_t m_nModelId;
		CVector m_vecPosition;
		float m_fRotation;
		uint8_t m_nPrimaryColor;
		uint8_t m_nSecondaryColor;
	};

	struct VehicleRemove
	{
		uint16_t m_nVehicleId;
	};

	struct VehicleIdleUpdate
	{
		uint16_t m_nVehicleId;
		CVector m_vecPosition;
		CVector	m_vecVelocity;
		CVector	m_vecRotation;
		CVector m_vecRoll;
		uint8_t m_nPrimaryColor;
		uint8_t m_nSecondaryColor;
		float m_nHealth;
		int8_t m_nPaintjob;
		float m_fPlaneGearState;
		bool m_bLocked;
	};

	struct VehicleDriverUpdate
	{
		uint16_t m_nPlayerId;
		uint16_t m_nVehicleId;
		VehicleSyncData m_syncData;
	};

	struct VehicleEnter
	{
		uint16_t m_nPlayerId;
		uint16_t m_nVehicleId;
		uint8_t m_nSeatId;
		bool m_bForce; // if true - put directly in vehicle (without any anim)
	};

	struct VehicleExit
	{
		uint16_t m_nPlayerId;
		bool m_bForce;
	};

	struct VehicleDamage
	{
		uint16_t m_nVehicleId;
		CDamageManager m_damageManager;
	};

	struct VehicleComponentAdd
	{
		uint16_t m_nVehicleId;
		int m_nComponentId;
	};

	struct VehicleComponentRemove
	{
		uint16_t m_nVehicleId;
		int m_nComponentId;
	};

	struct VehiclePassengerUpdate
	{
		uint16_t m_nPlayerId;
		uint16_t m_nVehicleId;
		uint8_t m_nPlayerHealth;
		uint8_t m_nPlayerArmour;
		uint8_t m_nWeapon;
		uint16_t m_nAmmoInClip;
		bool m_bDriveby;
		float m_fAimX;
		float m_fAimY;
	};

	struct PlayerChatMessage
	{
		uint16_t m_nPlayerId;
		char m_message[128 + 1];
	};

	struct PedSpawn
	{
		uint16_t m_nPedId;
		int16_t m_nModelId;
		uint8_t m_nPedType;
		CVector m_vecPosition;
		uint8_t m_nCreatedBy;
	};

	struct PedRemove
	{
		uint16_t m_nPedId;
	};

	struct PedOnFoot
	{
		uint16_t m_nPedId;
		PedSyncData m_syncData;
	};
	
	struct GameWeatherTime
	{
		uint8_t m_nNewWeather;
		uint8_t m_nOldWeather;
		uint8_t m_nForcedWeather;
		uint8_t m_nCurrentMonth;
		uint8_t m_nCurrentDay;
		uint8_t m_nCurrentHour;
		uint8_t m_nCurrentMinute;
		uint32_t m_nGameTickCount;
	};

	struct PedRemoveTask
	{
		uint16_t m_nPedId;
		eTaskType m_nTaskId;
	};

	struct PlayerKeySync
	{
		uint16_t m_nPlayerId;
		ÑCompressedControllerState m_controllerState;
	};

	struct PedDriverUpdate
	{
		uint16_t m_nPedId;
		uint16_t m_nVehicleId;
		uint8_t m_nPedHealth;
		uint8_t m_nPedArmour;
		uint8_t m_nWeapon;
		uint16_t m_nAmmoInClip;
		VehicleSyncData m_syncData;
		uint8_t m_autoPilot[92];
		float m_fGasPedal;
		float m_fBreakPedal;
		float m_fSteerAngle;
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