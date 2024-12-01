#pragma once

#ifndef _CPEDMANAGER_H_
	#define _CPEDMANAGER_H_
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>

#include "CPed.h"
#include "CPlayerManager.h"
#include "CVehicleManager.h"

class CPedManager
{
	public:
		CPedManager();
		
		static std::vector<CPed*> m_pPeds;
		static void Add(CPed* ped);
		static void Remove(CPed* ped);
		static CPed* GetPed(int pedid);
		
		~CPedManager();

};

class CPedPackets
{
	public:
		CPedPackets();

		struct PedSpawn
		{
			int pedid;
			short modelId;
			unsigned char pedType;
			CVector pos;
			unsigned char createdBy;

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				if (!CPlayerManager::GetPlayer(peer)->m_bIsHost)
					return;

				CPedPackets::PedSpawn* packet = (CPedPackets::PedSpawn*)data;
				CNetwork::SendPacketToAll(CPacketsID::PED_SPAWN, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);

				CPed* ped = new CPed(packet->pedid, packet->modelId, packet->pedType, packet->pos, packet->createdBy);

				CPedManager::Add(ped);
			}
		};

		struct PedRemove
		{
			int pedid;

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				if (!CPlayerManager::GetPlayer(peer)->m_bIsHost)
					return;

				CPedPackets::PedRemove* packet = (CPedPackets::PedRemove*)data;

				CPed* ped = CPedManager::GetPed(packet->pedid);

				if (!ped)
					return;

				CNetwork::SendPacketToAll(CPacketsID::PED_REMOVE, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);

				CPedManager::Remove(ped);
			}
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
			float lookDirection = 0.0f;
			struct
			{
				unsigned char moveState : 3;
				unsigned char ducked : 1;
				unsigned char aiming : 1;
			};
			CVector weaponAim;

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				if (!CPlayerManager::GetPlayer(peer)->m_bIsHost)
					return;

				CPedPackets::PedOnFoot* packet = (CPedPackets::PedOnFoot*)data;

				CPed* ped = CPedManager::GetPed(packet->pedid);

				if (ped)
				{
					ped->m_vecPos = packet->pos;
					CNetwork::SendPacketToAll(CPacketsID::PED_ONFOOT, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED, peer);
				}
			}
		};



		struct PedRemoveTask
		{
			int pedid;
			int taskid; // eTaskType
		};

		
		struct PedAddTask
		{
			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CNetwork::SendPacketToAll(CPacketsID::PED_ADD_TASK, data, size, ENET_PACKET_FLAG_RELIABLE, peer);
			}
		};

		struct PedDriverUpdate
		{
			int pedid;
			int vehicleid;
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
			float gasPedal;
			float breakPedal;
			float steerAngle;

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CPedPackets::PedDriverUpdate* packet = (CPedPackets::PedDriverUpdate*)data;
				CNetwork::SendPacketToAll(CPacketsID::PED_DRIVER_UPDATE, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED, peer);

				CVehicle* vehicle = CVehicleManager::GetVehicle(packet->vehicleid);

				if (vehicle == nullptr) // TODO: create vehicle
					return;

				vehicle->m_vecPosition = packet->pos;
				vehicle->m_vecRotation = packet->rot;
			}
		};

		struct PedShotSync
		{
			int pedid;
			CVector origin;
			CVector effect;
			CVector target;

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CPedPackets::PedShotSync* packet = (CPedPackets::PedShotSync*)data;
				CNetwork::SendPacketToAll(CPacketsID::PED_SHOT_SYNC, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
			}
		};


		~CPedPackets();
};
#endif