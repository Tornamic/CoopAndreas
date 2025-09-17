#pragma once

#include "stdafx.h"
#include <CNetworkEntitySerializer.h>

enum eNetworkEntityType : uint8_t;
enum ePlayerDisconnectReason : uint8_t;

enum CPacketsID : unsigned short
{
	PLAYER_CONNECTED,
	PLAYER_DISCONNECTED,
	PLAYER_ONFOOT,
	PLAYER_BULLET_SHOT,
	PLAYER_HANDSHAKE,
	PLAYER_PLACE_WAYPOINT,
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
	PED_SHOT_SYNC,
	PED_PASSENGER_UPDATE,
	PLAYER_AIM_SYNC,
	VEHICLE_CONFIRM,
	PED_CONFIRM,
	PLAYER_STATS,
	REBUILD_PLAYER,
	RESPAWN_PLAYER,
	ASSIGN_VEHICLE,
	ASSIGN_PED,
	MASS_PACKET_SEQUENCE,
	START_CUTSCENE,
	SKIP_CUTSCENE,
	OPCODE_SYNC,
	ON_MISSION_FLAG_SYNC,
	UPDATE_ENTITY_BLIP,
	REMOVE_ENTITY_BLIP,
	ADD_MESSAGE_GXT,
	REMOVE_MESSAGE_GXT,
	CLEAR_ENTITY_BLIPS,
	PLAY_MISSION_AUDIO,
	UPDATE_CHECKPOINT,
	REMOVE_CHECKPOINT,
	ENEX_SYNC,
	CREATE_STATIC_BLIP,
	SET_VEHICLE_CREATED_BY,
	SET_PLAYER_TASK,
	PED_SAY,
	PED_CLAIM_ON_RELEASE,
	PED_CANCEL_CLAIM,
	PED_RESET_ALL_CLAIMS,
	PED_TAKE_HOST,
	PERFORM_TASK_SEQUENCE,
	ADD_PROJECTILE,
	TAG_UPDATE,
	UPDATE_ALL_TAGS,
	TELEPORT_PLAYER_SCRIPTED,
	PACKET_ID_MAX
};

class CPackets
{
public:
	static int GetPacketSize(CPacketsID id)
	{
		static std::array<int, PACKET_ID_MAX> m_nPacketSize =
		{
			sizeof(PlayerConnected), // PLAYER_CONNECTED
			sizeof(PlayerDisconnected), // PLAYER_DISCONNECTED
			sizeof(PlayerOnFoot), // PLAYER_ONFOOT
			sizeof(PlayerBulletShot), // PLAYER_BULLET_SHOT
			sizeof(PlayerHandshake), // PLAYER_HANDSHAKE
			sizeof(PlayerPlaceWaypoint), // PLAYER_PLACE_WAYPOINT
			sizeof(VehicleSpawn), // VEHICLE_SPAWN
			sizeof(PlayerSetHost), // PLAYER_SET_HOST
			sizeof(AddExplosion), // ADD_EXPLOSION
			sizeof(VehicleRemove), // VEHICLE_REMOVE
			sizeof(VehicleIdleUpdate), // VEHICLE_IDLE_UPDATE
			sizeof(VehicleDriverUpdate), // VEHICLE_DRIVER_UPDATE
			sizeof(VehicleEnter), // VEHICLE_ENTER
			sizeof(VehicleExit), // VEHICLE_EXIT
			sizeof(VehicleDamage), // VEHICLE_DAMAGE
			sizeof(VehicleComponentAdd), // VEHICLE_COMPONENT_ADD
			sizeof(VehicleComponentRemove), // VEHICLE_COMPONENT_REMOVE
			sizeof(VehiclePassengerUpdate), // VEHICLE_PASSENGER_UPDATE
			sizeof(PlayerChatMessage), // PLAYER_CHAT_MESSAGE
			sizeof(PedSpawn), // PED_SPAWN
			sizeof(PedRemove), // PED_REMOVE
			sizeof(PedOnFoot), // PED_ONFOOT
			sizeof(GameWeatherTime), // GAME_WEATHER_TIME
			0, // PED_ADD_TASK
			sizeof(PedRemoveTask), // PED_REMOVE_TASK
			sizeof(PlayerKeySync), // PLAYER_KEY_SYNC
			sizeof(PedDriverUpdate), // PED_DRIVER_UPDATE
			sizeof(PedShotSync), // PED_SHOT_SYNC
			sizeof(PedPassengerSync), // PED_PASSENGER_UPDATE
			sizeof(PlayerAimSync), // PLAYER_AIM_SYNC
			sizeof(VehicleConfirm), // VEHICLE_CONFIRM
			sizeof(PedConfirm), // PED_CONFIRM
			sizeof(PlayerStats), // PLAYER_STATS
			sizeof(RebuildPlayer), // REBUILD_PLAYER
			sizeof(RespawnPlayer), // RESPAWN_PLAYER
			sizeof(AssignVehicleSyncer), // ASSIGN_VEHICLE
			sizeof(AssignPedSyncer), // ASSIGN_PED
			0, // MASS_PACKET_SEQUENCE
			sizeof(StartCutscene), // START_CUTSCENE,
			sizeof(SkipCutscene), // SKIP_CUTSCENE,
			0, // OPCODE_SYNC,
			sizeof(OnMissionFlagSync), // ON_MISSION_FLAG_SYNC,
			sizeof(UpdateEntityBlip), // UPDATE_ENTITY_BLIP,
			sizeof(RemoveEntityBlip), // REMOVE_ENTITY_BLIP,
			sizeof(AddMessageGXT), // ADD_MESSAGE_GXT,
			sizeof(RemoveMessageGXT), // REMOVE_MESSAGE_GXT,
			sizeof(ClearEntityBlips), // CLEAR_ENTITY_BLIPS,
			sizeof(PlayMissionAudio), // PLAY_MISSION_AUDIO,
			sizeof(UpdateCheckpoint), // UPDATE_CHECKPOINT,
			sizeof(RemoveCheckpoint), // REMOVE_CHECKPOINT,
			0, // ENEX_SYNC,
			sizeof(CreateStaticBlip), // CREATE_STATIC_BLIP,
		};

		return m_nPacketSize[id];
	}

	struct PlayerConnected
	{
		int id;
		bool isAlreadyConnected; // prevents spam in the chat when connecting by distinguishing already connected players from newly joined ones
		char name[32 + 1];
		uint32_t version;
	};

	struct PlayerDisconnected
	{
		int id;
		ePlayerDisconnectReason reason;
		uint32_t version;
	};

	struct PlayerOnFoot
	{
		int id = 0;
		CVector position = CVector();
		CVector velocity = CVector();
		float currentRotation = 0.0f;
		float aimingRotation = 0.0f;
		unsigned char health = 100;
		unsigned char armour = 0;
		unsigned char weapon = 0;
		unsigned char weaponState = 0;
		unsigned short ammo = 0;
		bool ducking = false;
		bool hasJetpack = false;
		char fightingStyle = 4;
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

	struct PlayerSetHost
	{
		int playerid;
	};

	struct AddExplosion
	{
		unsigned char type;
		CVector pos;
		int time;
		bool usesSound;
		float cameraShake;
		bool isVisible;
	};

	struct VehicleSpawn
	{
		int vehicleid;
		unsigned char tempid;
		unsigned short modelid;
		CVector pos;
		float rot;
		unsigned char color1;
		unsigned char color2;
		unsigned char createdBy;
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
		CVector turnSpeed;
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
		unsigned char playerHealth;
		unsigned char playerArmour;
		unsigned char weapon;
		unsigned short ammo;
		unsigned char color1;
		unsigned char color2;
		float health;
		char paintjob;
		float bikeLean;
		unsigned short miscComponentAngle; // hydra thrusters
		float planeGearState;
		unsigned char locked;
	};

	struct VehicleEnter
	{
		int playerid;
		int vehicleid;
		unsigned char seatid : 3;
		unsigned char force : 1;
		unsigned char passenger : 1;
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
		unsigned char playerHealth;
		unsigned char playerArmour;
		unsigned char weapon;
		unsigned short ammo;
		unsigned char driveby;
		unsigned char seatid;
	};

	struct PlayerChatMessage
	{
		int playerid;
		wchar_t message[CChat::MAX_MESSAGE_SIZE+1];
	};

	struct PedSpawn
	{
		int pedid;
		unsigned char tempid;
		short modelId;
		unsigned char pedType;
		CVector pos;
		unsigned char createdBy;
		char specialModelName[8];
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
		unsigned char health = 100;
		unsigned char armour = 0;
		unsigned char weapon = 0;
		unsigned short ammo = 0;
		float aimingRotation = 0.0f;
		float currentRotation = 0.0f;
		int lookDirection = 0;
		struct
		{
			unsigned char moveState : 3;
			unsigned char ducked : 1;
			unsigned char aiming : 1;
		};
		unsigned char fightingStyle = 4;
		CVector weaponAim;
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
		int pedid;
		eTaskType taskid;
	};

	struct PlayerKeySync
	{
		int playerid;
		CCompressedControllerState newState;
	};

	struct PedDriverUpdate
	{
		int pedid;
		int vehicleid;
		CVector pos;
		CVector rot;
		CVector roll;
		CVector velocity;
		CVector turnSpeed;
		unsigned char pedHealth;
		unsigned char pedArmour;
		unsigned char weapon;
		unsigned short ammo;
		unsigned char color1;
		unsigned char color2;
		float health;
		char paintjob;
		float bikeLean;
		union
		{
			float controlPedaling;
			float planeGearState;
		};
		unsigned char locked;
		float gasPedal;
		float breakPedal;
		uint8_t drivingStyle;
		uint8_t carMission;
		int8_t cruiseSpeed;
		uint8_t ctrlFlags;
		uint8_t movementFlags;
		int targetVehicleId;
		CVector destinationCoors;
	};

	struct PedShotSync
	{
		int pedid;
		CVector origin;
		CVector effect;
		CVector target;
	};

	struct PedPassengerSync
	{
		int pedid;
		int vehicleid;
		unsigned char health;
		unsigned char armour;
		unsigned char weapon;
		unsigned short ammo;
		unsigned char seatid;
	};

	struct PlayerAimSync
	{
		int playerid;
		unsigned char cameraMode;
		unsigned char weaponCameraMode;
		float cameraFov;
		CVector front;
		CVector	source;
		CVector	up;
		float lookPitch;
		float orientation;
	};

	struct VehicleConfirm
	{
		unsigned char tempid = 255;
		int vehicleid;
	};

	struct PedConfirm
	{
		unsigned char tempid = 255;
		int pedid;
	};

	struct PlayerStats
	{
		int playerid;
		float stats[14];
	};

	struct RebuildPlayer
	{
		int playerid;
		CPedClothesDesc clothesData;
	};

	struct AssignVehicleSyncer
	{
		int vehicleid;
	};

	struct AssignPedSyncer
	{
		int pedid;
	};

	struct RespawnPlayer
	{
		int playerid;
	};

	struct StartCutscene
	{
		char name[8];
		uint8_t currArea; // AKA interior
	};

	struct SkipCutscene
	{
		int playerid;
		int votes; // temporary unused
	};

	struct OnMissionFlagSync 
	{
		uint8_t bOnMission : 1;
	};

	struct UpdateEntityBlip
	{
		int playerid;
		eNetworkEntityType entityType;
		int entityId;
		bool isFriendly;
		uint8_t color;
		uint8_t display;
		uint8_t scale;
	};

	struct RemoveEntityBlip
	{
		int playerid;
		eNetworkEntityType entityType;
		int entityId;
	};

	struct AddMessageGXT
	{
		int playerid;
		// 0 - COMMAND_PRINT
		// 1 - COMMAND_PRINT_BIG
		// 2 - COMMAND_PRINT_NOW
		// 3 - COMMAND_PRINT_HELP
		uint8_t type; 
		uint32_t time;
		uint8_t flag;
		char gxt[8];
	};

	struct RemoveMessageGXT
	{
		int playerid;
		char gxt[8];
	};

	struct ClearEntityBlips
	{
		int playerid;
	};

	struct PlayMissionAudio
	{
		uint8_t slotid;
		int audioid;
	};

	struct UpdateCheckpoint
	{
		int playerid;
		CVector position;
		CVector radius;
	};

	struct RemoveCheckpoint
	{
		int playerid;
	};

	struct CreateStaticBlip
	{
		CVector position;
		int8_t sprite;
		uint8_t display : 2;
		uint8_t type : 1; // 0 - BLIP_CONTACT_POINT, 1 - BLIP_COORD
		uint8_t trackingBlip : 1;
		uint8_t shortRange : 1;
		uint8_t friendly : 1; // It is affected by BLIP_COLOUR_THREAT.   
		uint8_t coordBlipAppearance : 2; // see eBlipAppearance
		uint8_t size : 3;
		uint8_t color : 4;
	};

	struct SetVehicleCreatedBy
	{
		int vehicleid;
		uint8_t createdBy;
	};

	struct SetPlayerTask
	{
		int playerid;
		int taskType;
		CVector position;
		float rotation;
		bool toggle;
	};

	struct PedSay
	{
		int entityid : 31;
		int isPlayer : 1;
		int16_t phraseId;
		int startTimeDelay;
		uint8_t overrideSilence : 1;
		uint8_t isForceAudible : 1;
		uint8_t isFrontEnd : 1;
	};

	struct PedClaimOnRelease
	{
		int pedid;
	};

	struct PedCancelClaim
	{
		int pedid;
	};

	struct PedResetAllClaims
	{
		int pedid;
	};

	struct PedTakeHost
	{
		int pedid;
		bool allowReturnToPreviousHost;
	};

	struct AddProjectile
	{
		CNetworkEntitySerializer creator;
		uint8_t projectileType; // eWeaponType
		CVector origin;
		float force;
		CVector dir;
		CNetworkEntitySerializer target;
	};

	struct TagUpdate
	{
		int16_t pos_x;
		int16_t pos_y;
		int16_t pos_z;
		uint8_t alpha;
	};

	struct UpdateAllTags
	{
		TagUpdate tags[150];
	};

	struct TeleportPlayerScripted
	{
		int playerid;
		CVector pos;
		float heading;
	};
};