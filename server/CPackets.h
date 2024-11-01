#pragma once
#include "stdafx.h"
#include "CControllerState.h"
#include "CNetwork.h"
#include "CNetworkPed.h"
#include "CNetworkPedManager.h"
#include "CNetworkPlayerManager.h"
#include "CNetworkVehicle.h"
#include "CNetworkVehicleManager.h"
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
	#pragma pack(1)
	struct PlayerOnFoot
	{
		uint16_t m_nPlayerId;
		PlayerSyncData m_syncData;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			// create packet
			CPackets::PlayerOnFoot* packet = (CPackets::PlayerOnFoot*)data;

			auto networkPlayer = CNetworkPlayerManager::Instance().Get(peer);
			// set packet`s m_nPlayerId, cuz incoming packet has id = 0
			packet->m_nPlayerId = networkPlayer->GetId();

			networkPlayer->GetSyncData() = packet->m_syncData;

			CNetwork::SendPacketToAll(ePacketType::PLAYER_ONFOOT, packet, sizeof *packet, ENET_PACKET_FLAG_UNSEQUENCED, peer);
		}
	};

	#pragma pack(1)
	struct PlayerBulletShot
	{
		uint16_t m_nPlayerId;
		uint16_t m_nTargetId;
		CVector m_vecStartPos;
		CVector m_vecEndPos;
		uint8_t m_colPoint[44]; // padding
		int32_t m_nIncrementalHit;
		uint8_t m_nTargetEntityType;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			// create packet
			CPackets::PlayerBulletShot* packet = (CPackets::PlayerBulletShot*)data;
			
			// set packet`s m_nPlayerId, cuz incoming packet has id = 0
			packet->m_nPlayerId = CNetworkPlayerManager::Instance().Get(peer)->GetId();

			CNetwork::SendPacketToAll(ePacketType::PLAYER_BULLET_SHOT, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED, peer);
		}
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

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			CPackets::PlayerPlaceWaypoint* packet = (CPackets::PlayerPlaceWaypoint*)data;
			packet->m_nPlayerId = CNetworkPlayerManager::Instance().Get(peer)->GetId();
			CNetwork::SendPacketToAll(ePacketType::PLAYER_PLACE_WAYPOINT, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
		}
	};

	struct PlayerGetName
	{
		uint16_t m_nPlayerId;
		char m_aName[32 + 1];

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			CPackets::PlayerGetName* packet = (CPackets::PlayerGetName*)data;
			CNetworkPlayer* player = CNetworkPlayerManager::Instance().Get(peer);
			packet->m_nPlayerId = player->GetId();
			player->SetName(packet->m_aName);
			printf("player %d now also know as %s\n", packet->m_nPlayerId, packet->m_aName);
			CNetwork::SendPacketToAll(ePacketType::PLAYER_GET_NAME, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
		}
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

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			CPackets::AddExplosion* packet = (CPackets::AddExplosion*)data;
			CNetwork::SendPacketToAll(ePacketType::ADD_EXPLOSION, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
		}
	};

	struct VehicleSpawn
	{
		uint16_t m_nVehicleId;
		int16_t m_nModelId;
		CVector m_vecPosition;
		float m_fRotation;
		uint8_t m_nPrimaryColor;
		uint8_t m_nSecondaryColor;
		
		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (!CNetworkPlayerManager::Instance().Get(peer)->IsHost())
				return;

			CPackets::VehicleSpawn* packet = (CPackets::VehicleSpawn*)data;
			CNetwork::SendPacketToAll(ePacketType::VEHICLE_SPAWN, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
			
			CNetworkVehicle* vehicle = new CNetworkVehicle(packet->m_nVehicleId, packet->m_nModelId);

			auto& syncData = vehicle->GetSyncData();

			syncData.m_vecPosition = packet->m_vecPosition;
			syncData.m_vecRotation.z = packet->m_fRotation; // possibly need convert to degrees
			syncData.m_nPrimaryColor = packet->m_nPrimaryColor;
			syncData.m_nSecondaryColor = packet->m_nSecondaryColor;

			CNetworkVehicleManager::Instance().Add(vehicle);
		}
	};

	struct VehicleRemove
	{
		uint16_t m_nVehicleId;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (!CNetworkPlayerManager::Instance().Get(peer)->IsHost())
				return;

			CPackets::VehicleRemove* packet = (CPackets::VehicleRemove*)data;
			CNetwork::SendPacketToAll(ePacketType::VEHICLE_REMOVE, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);

			CNetworkVehicle* vehicle = CNetworkVehicleManager::Instance().Get(packet->m_nVehicleId);
			CNetworkVehicleManager::Instance().Remove(vehicle);
		}
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

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (!CNetworkPlayerManager::Instance().Get(peer)->IsHost())
				return;

			CPackets::VehicleIdleUpdate* packet = (CPackets::VehicleIdleUpdate*)data;
			CNetwork::SendPacketToAll(ePacketType::VEHICLE_IDLE_UPDATE, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED, peer);

			CNetworkVehicle* vehicle = CNetworkVehicleManager::Instance().Get(packet->m_nVehicleId);
			
			if (!vehicle)
				return;

			auto& syncData = vehicle->GetSyncData();
			syncData.m_vecPosition	   = packet->m_vecPosition;
			syncData.m_vecVelocity	   = packet->m_vecVelocity;
			syncData.m_vecRotation	   = packet->m_vecRotation;
			syncData.m_vecRoll		   = packet->m_vecRoll;
			syncData.m_nPrimaryColor   = packet->m_nPrimaryColor;
			syncData.m_nSecondaryColor = packet->m_nSecondaryColor;
			syncData.m_nHealth		   = packet->m_nHealth;
			syncData.m_nPaintjob	   = packet->m_nPaintjob;
			syncData.m_fPlaneGearState = packet->m_fPlaneGearState;
			syncData.m_bLocked		   = packet->m_bLocked;
		}
	};

	struct VehicleDriverUpdate
	{
		uint16_t m_nPlayerId;
		uint16_t m_nVehicleId;
		VehicleSyncData m_syncData;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			CPackets::VehicleDriverUpdate* packet = (CPackets::VehicleDriverUpdate*)data;
			packet->m_nPlayerId = CNetworkPlayerManager::Instance().Get(peer)->GetId();
			CNetwork::SendPacketToAll(ePacketType::VEHICLE_DRIVER_UPDATE, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED, peer);

			CNetworkVehicle* vehicle = CNetworkVehicleManager::Instance().Get(packet->m_nVehicleId);

			vehicle->GetSyncData() = packet->m_syncData;
		}
	};

	struct VehicleEnter
	{
		uint16_t m_nPlayerId;
		uint16_t m_nVehicleId;
		uint8_t m_nSeatId;
		bool m_bForce; // if true - put directly in vehicle (without any anim)

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			CPackets::VehicleEnter* packet = (CPackets::VehicleEnter*)data;
			CNetworkPlayer* player = CNetworkPlayerManager::Instance().Get(peer);

			CNetworkVehicle* vehicle = CNetworkVehicleManager::Instance().Get(packet->m_nVehicleId);
			player->SetSeatId(packet->m_nSeatId);
			player->SetVehicle(vehicle);

			packet->m_nPlayerId = player->GetId();

			CNetwork::SendPacketToAll(ePacketType::VEHICLE_ENTER, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);

			vehicle->m_aOccupants[packet->m_nSeatId] = player;
		}
	};

	struct VehicleExit
	{
		uint16_t m_nPlayerId;
		bool m_bForce;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			CPackets::VehicleExit* packet = (CPackets::VehicleExit*)data;
			CNetworkPlayer* player = CNetworkPlayerManager::Instance().Get(peer);
			packet->m_nPlayerId = player->GetId();
			CNetwork::SendPacketToAll(ePacketType::VEHICLE_EXIT, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);

			CNetworkVehicle* vehicle = player->GetVehicle();

			vehicle->m_aOccupants[player->GetSeatId()] = nullptr;
			player->SetSeatId(-1);
			player->SetVehicle(nullptr);
		}
	};

	struct VehicleDamage
	{
		uint16_t m_nVehicleId;
		unsigned char damageManager_padding[23];

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			CPackets::VehicleDamage* packet = (CPackets::VehicleDamage*)data;
			memcpy(CNetworkVehicleManager::Instance().Get(packet->m_nVehicleId)->m_damageManager_padding, packet->damageManager_padding, 23);
			CNetwork::SendPacketToAll(ePacketType::VEHICLE_DAMAGE, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
		}
	};

	struct VehicleComponentAdd
	{
		uint16_t m_nVehicleId;
		int m_nComponentId;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			CPackets::VehicleComponentAdd* packet = (CPackets::VehicleComponentAdd*)data;
			CNetwork::SendPacketToAll(ePacketType::VEHICLE_COMPONENT_ADD, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);

			CNetworkVehicle* vehicle = CNetworkVehicleManager::Instance().Get(packet->m_nVehicleId);
			vehicle->m_aComponents.push_back(packet->m_nComponentId);
		}
	};

	struct VehicleComponentRemove
	{
		uint16_t m_nVehicleId;
		int m_nComponentId;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			CPackets::VehicleComponentAdd* packet = (CPackets::VehicleComponentAdd*)data;
			CNetwork::SendPacketToAll(ePacketType::VEHICLE_COMPONENT_REMOVE, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);

			CNetworkVehicle* vehicle = CNetworkVehicleManager::Instance().Get(packet->m_nVehicleId);

			auto it = std::find(vehicle->m_aComponents.begin(), vehicle->m_aComponents.end(), packet->m_nComponentId);
			if (it != vehicle->m_aComponents.end())
				vehicle->m_aComponents.erase(it);
		}
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

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			CPackets::VehiclePassengerUpdate* packet = (CPackets::VehiclePassengerUpdate*)data;
			auto networkPlayer = CNetworkPlayerManager::Instance().Get(peer);
			packet->m_nPlayerId = networkPlayer->GetId();

			auto& syncData = networkPlayer->GetSyncData();
			syncData.m_nHealth = packet->m_nPlayerHealth;
			syncData.m_nArmour = packet->m_nPlayerArmour;
			syncData.m_nCurrentWeapon = packet->m_nWeapon;
			syncData.m_nAmmoInClip = packet->m_nAmmoInClip;
			syncData.m_fAimX = packet->m_fAimX;
			syncData.m_fAimY = packet->m_fAimY;

			CNetwork::SendPacketToAll(ePacketType::VEHICLE_PASSENGER_UPDATE, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED, peer);
		}
	};

	struct PlayerChatMessage
	{
		uint16_t m_nPlayerId;
		char m_message[128 + 1];

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			CPackets::PlayerChatMessage* packet = (CPackets::PlayerChatMessage*)data;
			packet->m_nPlayerId = CNetworkPlayerManager::Instance().Get(peer)->GetId();
			CNetwork::SendPacketToAll(ePacketType::PLAYER_CHAT_MESSAGE, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
		}
	};

	struct PedSpawn
	{
		uint16_t m_nPedId;
		int16_t m_nModelId;
		uint8_t m_nPedType;
		CVector m_vecPosition;
		uint8_t m_nCreatedBy;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (!CNetworkPlayerManager::Instance().Get(peer)->m_bIsHost)
				return;

			CPackets::PedSpawn* packet = (CPackets::PedSpawn*)data;
			CNetwork::SendPacketToAll(ePacketType::PED_SPAWN, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);

			CNetworkPed* ped = new CNetworkPed(packet->m_nPedId, packet->m_nModelId, packet->m_nPedType, packet->m_nCreatedBy);
			ped->GetSyncData().m_vecPosition = packet->m_vecPosition;

			CNetworkPedManager::Instance().Add(ped);
		}
	};

	struct PedRemove
	{
		uint16_t m_nPedId;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (!CNetworkPlayerManager::Instance().Get(peer)->m_bIsHost)
				return;

			CPackets::PedRemove* packet = (CPackets::PedRemove*)data;

			CNetworkPed* ped = CNetworkPedManager::Instance().Get(packet->m_nPedId);

			if (!ped)
				return;

			CNetwork::SendPacketToAll(ePacketType::PED_REMOVE, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);

			CNetworkPedManager::Instance().Remove(ped);
		}
	};

	struct PedOnFoot
	{
		uint16_t m_nPedId;
		PedSyncData m_syncData;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (!CNetworkPlayerManager::Instance().Get(peer)->m_bIsHost)
				return;

			CPackets::PedOnFoot* packet = (CPackets::PedOnFoot*)data;

			CNetworkPed* ped = CNetworkPedManager::Instance().Get(packet->m_nPedId);

			if (ped)
			{
				ped->GetSyncData() = packet->m_syncData;
				CNetwork::SendPacketToAll(ePacketType::PED_ONFOOT, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED, peer);
			}
		}
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

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (!CNetworkPlayerManager::Instance().Get(peer)->m_bIsHost)
				return;

			CPackets::GameWeatherTime* packet = (CPackets::GameWeatherTime*)data;
			CNetwork::SendPacketToAll(ePacketType::GAME_WEATHER_TIME, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
		}
	};

	struct PedRemoveTask
	{
		uint16_t m_nPedId;
		int32_t m_nTaskId; // eTaskType
	};

	struct PlayerKeySync
	{
		uint16_t m_nPlayerId;
		ÑCompressedControllerState m_controllerState;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			CPackets::PlayerKeySync* packet = (CPackets::PlayerKeySync*)data;
			packet->m_nPlayerId = CNetworkPlayerManager::Instance().Get(peer)->GetId();
			CNetwork::SendPacketToAll(ePacketType::PLAYER_KEY_SYNC, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
		}
	};

	struct PedAddTask
	{
		static void Handle(ENetPeer* peer, void* data, int size)
		{
			CNetwork::SendPacketToAll(ePacketType::PED_ADD_TASK, data, size, ENET_PACKET_FLAG_RELIABLE, peer);
		}
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

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			CPackets::PedDriverUpdate* packet = (CPackets::PedDriverUpdate*)data;
			CNetwork::SendPacketToAll(ePacketType::PED_DRIVER_UPDATE, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED, peer);

			CNetworkVehicle* vehicle = CNetworkVehicleManager::Instance().Get(packet->m_nVehicleId);

			if (vehicle == nullptr) // TODO: create vehicle
				return;

			vehicle->GetSyncData() = packet->m_syncData;

			CNetworkPed* ped = CNetworkPedManager::Instance().Get(packet->m_nPedId);
			auto& pedSyncData = ped->GetSyncData();
			pedSyncData.m_nHealth = packet->m_nPedHealth;
			pedSyncData.m_nArmour = packet->m_nPedArmour;
			pedSyncData.m_nCurrentWeapon = packet->m_nWeapon;
			pedSyncData.m_nAmmoInClip = packet->m_nAmmoInClip;
			
		}
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