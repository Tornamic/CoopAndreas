#pragma once

#ifndef _CVEHICLEMANAGER_H_
	#define _CVEHICLEMANAGER_H_
#include "CVehicle.h"

class CVehicleManager
{
	public:
		CVehicleManager();
		
		static std::vector<CVehicle*> m_pVehicles;
		static void Add(CVehicle* vehicle);
		static void Remove(CVehicle* vehicle);
		static CVehicle* GetVehicle(int vehicleid);
		
		~CVehicleManager();

};

class CVehiclePackets
{
	public:
		CVehiclePackets();

		struct VehicleSpawn
		{
			int vehicleid;
			unsigned short modelid;
			CVector pos;
			float rot;
			unsigned char color1;
			unsigned char color2;
		
			static void Handle(ENetPeer* peer, void* data, int size)
			{
				if (!CPlayerManager::GetPlayer(peer)->m_bIsHost)
					return;


				CVehiclePackets::VehicleSpawn* packet = (CVehiclePackets::VehicleSpawn*)data;
				CNetwork::SendPacketToAll(CPacketsID::VEHICLE_SPAWN, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
				
				CVehicle* vehicle = new CVehicle(packet->vehicleid, packet->modelid, packet->pos, packet->rot);

				vehicle->m_nPrimaryColor = packet->color1;
				vehicle->m_nSecondaryColor = packet->color2;

				CVehicleManager::Add(vehicle);
			}
		};

		struct VehicleRemove
		{
			int vehicleid;

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				if (!CPlayerManager::GetPlayer(peer)->m_bIsHost)
					return;

				CVehiclePackets::VehicleRemove* packet = (CVehiclePackets::VehicleRemove*)data;
				CNetwork::SendPacketToAll(CPacketsID::VEHICLE_REMOVE, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);

				CVehicle* vehicle = CVehicleManager::GetVehicle(packet->vehicleid);
				CVehicleManager::Remove(vehicle);
			}
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

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				if (!CPlayerManager::GetPlayer(peer)->m_bIsHost)
					return;

				CVehiclePackets::VehicleIdleUpdate* packet = (CVehiclePackets::VehicleIdleUpdate*)data;
				CNetwork::SendPacketToAll(CPacketsID::VEHICLE_IDLE_UPDATE, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED, peer);

				CVehicle* vehicle = CVehicleManager::GetVehicle(packet->vehicleid);
			
				if (!vehicle)
					return;

				vehicle->m_vecPosition = packet->pos;
				vehicle->m_vecRotation = packet->rot;
			}
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
			float turretAimHorizontal;
			float turretAimVertical;
			unsigned short miscComponentAngle; // hydra thrusters
			float planeGearState;
			unsigned char locked;

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CVehiclePackets::VehicleDriverUpdate* packet = (CVehiclePackets::VehicleDriverUpdate*)data;
				packet->playerid = CPlayerManager::GetPlayer(peer)->m_iPlayerId;
				CNetwork::SendPacketToAll(CPacketsID::VEHICLE_DRIVER_UPDATE, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED, peer);

				CVehicle* vehicle = CVehicleManager::GetVehicle(packet->vehicleid);

				vehicle->m_vecPosition = packet->pos;
				vehicle->m_vecRotation = packet->rot;
			}
		};

		struct VehicleEnter
		{
			int playerid;
			int vehicleid;
			unsigned char seatid;
			bool force; // if true - put directly in vehicle (without any anim)

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CVehiclePackets::VehicleEnter* packet = (CVehiclePackets::VehicleEnter*)data;
				CPlayer* player = CPlayerManager::GetPlayer(peer);
				player->m_nSeatId = packet->seatid;
				player->m_nVehicleId = packet->vehicleid;
				packet->playerid = player->m_iPlayerId;
				CNetwork::SendPacketToAll(CPacketsID::VEHICLE_ENTER, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);

				CVehicle* vehicle = CVehicleManager::GetVehicle(packet->vehicleid);
				//vehicle->m_pPlayers[packet->seatid] = player;
			}
		};

		struct VehicleExit
		{
			int playerid;
			bool force;

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CVehiclePackets::VehicleExit* packet = (CVehiclePackets::VehicleExit*)data;
				CPlayer* player = CPlayerManager::GetPlayer(peer);
				packet->playerid = player->m_iPlayerId;
				CNetwork::SendPacketToAll(CPacketsID::VEHICLE_EXIT, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);

				CVehicle* vehicle = CVehicleManager::GetVehicle(player->m_nVehicleId);
				vehicle->m_pPlayers[player->m_nSeatId] = nullptr;
				player->m_nSeatId = -1;
				player->m_nVehicleId = -1;
			}
		};

		struct VehicleDamage
		{
			int vehicleid;
			unsigned char damageManager_padding[23];

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CVehiclePackets::VehicleDamage* packet = (CVehiclePackets::VehicleDamage*)data;
				memcpy(CVehicleManager::GetVehicle(packet->vehicleid)->m_damageManager_padding, packet->damageManager_padding, 23);
				CNetwork::SendPacketToAll(CPacketsID::VEHICLE_DAMAGE, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
			}
		};

		struct VehicleComponentAdd
		{
			int vehicleid;
			int componentid;

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CVehiclePackets::VehicleComponentAdd* packet = (CVehiclePackets::VehicleComponentAdd*)data;
				CNetwork::SendPacketToAll(CPacketsID::VEHICLE_COMPONENT_ADD, packet, sizeof(* packet), ENET_PACKET_FLAG_RELIABLE, peer);

				CVehicle* vehicle = CVehicleManager::GetVehicle(packet->vehicleid);
				vehicle->m_pComponents.push_back(packet->componentid);
			}
		};

		struct VehicleComponentRemove
		{
			int vehicleid;
			int componentid;

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CVehiclePackets::VehicleComponentRemove* packet = (CVehiclePackets::VehicleComponentRemove*)data;
				CNetwork::SendPacketToAll(CPacketsID::VEHICLE_COMPONENT_REMOVE, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);

				CVehicle* vehicle = CVehicleManager::GetVehicle(packet->vehicleid);

				auto it = std::find(vehicle->m_pComponents.begin(), vehicle->m_pComponents.end(), packet->componentid);
				if (it != vehicle->m_pComponents.end())
					vehicle->m_pComponents.erase(it);
			}
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
			CVector aim;

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CVehiclePackets::VehiclePassengerUpdate* packet = (CVehiclePackets::VehiclePassengerUpdate*)data;
				packet->playerid = CPlayerManager::GetPlayer(peer)->m_iPlayerId;
				CNetwork::SendPacketToAll(CPacketsID::VEHICLE_PASSENGER_UPDATE, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED, peer);
			}
		};




		~CVehiclePackets();
};

#endif