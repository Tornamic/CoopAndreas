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
	static std::vector<CPed*> m_pPeds;
	static void Add(CPed* ped);
	static void Remove(CPed* ped);
	static CPed* GetPed(int pedid);
	static int GetFreeId();
	static void RemoveAllHostedAndNotify(CPlayer* player);

	static inline char ms_aszAllowedSpecialActors[52][8] =
	{
		"ANDRE", "BBTHIN", "BB","CAT","CESAR","COPGRL1","COPGRL2","COPGRL3",
		"CLAUDE","CROGRL1","CROGRL2","CROGRL3","DWAYNE","EMMET","FORELLI","GANGRL1",
		"GANGRL2","GANGRL3","GUNGRL1","GUNGRL2","GUNGRL3", "HERN","JANITOR","JETHRO",
		"JIZZY","KENDL","MACCER","MADDOGG","MECGRL1","MECGRL2","MECGRL3","NURGRL1",
		"NURGRL2","NURGRL3","OGLOC","PAUL","PULASKI","ROSE","RYDER1","RYDER2",
		"RYDER3","SINDACO","SMOKE","SMOKEV","SUZIE","SWEET","TBONE","TENPEN",
		"TORINO","TRUTH","WUZIMU","ZERO"
	};
};

class CPedPackets
{
	public:
#pragma pack(1)
		struct PedSpawn
		{
			int pedid;
			unsigned char tempid;
			short modelId;
			unsigned char pedType;
			CVector pos;
			unsigned char createdBy;
			char specialModelName[8];

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				auto player = CPlayerManager::GetPlayer(peer);

				if (!player)
					return;

				CPedPackets::PedSpawn* packet = (CPedPackets::PedSpawn*)data;

				if (packet->modelId > 311 || packet->modelId < 1)
				{
					return;
				}

				if (packet->modelId >= 290 && packet->modelId <= 299)
				{
					bool isSpecialModelValid = false;

					for (int i = 0; i < 52; i++)
					{
						if (_strnicmp(packet->specialModelName, CPedManager::ms_aszAllowedSpecialActors[i], strlen(CPedManager::ms_aszAllowedSpecialActors[i])))
						{
							isSpecialModelValid = true;
							break;
						}
					}

					if(!isSpecialModelValid)
						return;
				}

				packet->pedid = CPedManager::GetFreeId();
				CNetwork::SendPacketToAll(CPacketsID::PED_SPAWN, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
		
				CPed* ped = new CPed(packet->pedid, player, packet->modelId, packet->pedType, packet->pos, packet->createdBy);
				strncpy_s(ped->m_szSpecialModelName, packet->specialModelName, 7);
				CPedManager::Add(ped);

				// send it back to the syncer of the ped so that he knows the id
				CPedPackets::PedConfirm pedConfirmPacket{};
				pedConfirmPacket.tempid = packet->tempid;
				pedConfirmPacket.pedid = packet->pedid;
				CNetwork::SendPacket(peer, CPacketsID::PED_CONFIRM, &pedConfirmPacket, sizeof pedConfirmPacket, ENET_PACKET_FLAG_RELIABLE);
			}
		};

		struct PedRemove
		{
			int pedid;

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CPedPackets::PedRemove* packet = (CPedPackets::PedRemove*)data;

				CPed* ped = CPedManager::GetPed(packet->pedid);

				if (!ped)
					return;
				
				auto player = CPlayerManager::GetPlayer(peer);

				if (ped->m_pSyncer != player)
				{
					std::cout << "[Alert] " + player->GetName() + " tries to delete someone else's pedestrian, possible hack or bug (please let us know)\n";
					return;
				}

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
			unsigned char weaponState = 0;
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

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CPedPackets::PedOnFoot* packet = (CPedPackets::PedOnFoot*)data;

				CPed* ped = CPedManager::GetPed(packet->pedid);

				if (ped)
				{
					auto player = CPlayerManager::GetPlayer(peer);

					if (ped->m_pSyncer != player)
					{
						std::cout << "[Alert] " + player->GetName() + " tries to sync (on foot) someone else's pedestrian, possible hack or bug (please let us know)\n";
						return;
					}

					ped->m_vecPos = packet->pos;
					CNetwork::SendPacketToAll(CPacketsID::PED_ONFOOT, packet, sizeof * packet, (ENetPacketFlag)0, peer);
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
				// TODO: protect
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

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CPedPackets::PedDriverUpdate* packet = (CPedPackets::PedDriverUpdate*)data;

				auto ped = CPedManager::GetPed(packet->pedid);
				auto player = CPlayerManager::GetPlayer(peer);

				if (ped && ped->m_pSyncer != player)
				{
					std::cout << "[Alert] " + player->GetName() + " tries to sync (driver) someone else's pedestrian, possible hack or bug (please let us know)\n";
					return;
				}

				CNetwork::SendPacketToAll(CPacketsID::PED_DRIVER_UPDATE, packet, sizeof * packet, (ENetPacketFlag)0, peer);

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

				auto ped = CPedManager::GetPed(packet->pedid);
				auto player = CPlayerManager::GetPlayer(peer);

				if (ped && ped->m_pSyncer != player)
				{
					//std::cout << "[Alert] " + player->GetName() + " tries to sync (shots) someone else's pedestrian, possible hack or bug (please let us know)\n";
					return;
				}

				CNetwork::SendPacketToAll(CPacketsID::PED_SHOT_SYNC, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
			}
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

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CPedPackets::PedPassengerSync* packet = (CPedPackets::PedPassengerSync*)data;

				auto ped = CPedManager::GetPed(packet->pedid);
				auto player = CPlayerManager::GetPlayer(peer);

				if (ped && ped->m_pSyncer != player)
				{
					std::cout << "[Alert] " + player->GetName() + " tries to sync (passenger) someone else's pedestrian, possible hack or bug (please let us know)\n";
					return;
				}

				CNetwork::SendPacketToAll(CPacketsID::PED_PASSENGER_UPDATE, packet, sizeof * packet, (ENetPacketFlag)0, peer);
			}
		};

		struct PedConfirm
		{
			unsigned char tempid = 255;
			int pedid;
		};

		~CPedPackets();
};
#endif