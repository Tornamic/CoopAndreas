#pragma once

#ifndef _CVEHICLEMANAGER_H_
	#define _CVEHICLEMANAGER_H_
#include "CVehicle.h"

class CVehicleManager
{
public:
	static std::vector<CVehicle*> m_pVehicles;
	static void Add(CVehicle* vehicle);
	static void Remove(CVehicle* vehicle);
	static CVehicle* GetVehicle(int vehicleid);
	static int GetFreeID();
	static void RemoveAllHostedAndNotify(CPlayer* player);
};

class CVehiclePackets
{
	public:
		CVehiclePackets();
		#pragma pack(1)
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

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				auto player = CPlayerManager::GetPlayer(peer);

				if (!player)
					return;

				// send received packet
				CVehiclePackets::VehicleSpawn* packet = (CVehiclePackets::VehicleSpawn*)data;

				if (packet->modelid > 611 || packet->modelid < 400)
				{
					return;
				}

				packet->vehicleid = CVehicleManager::GetFreeID(); // find free vehicle id
				CNetwork::SendPacketToAll(CPacketsID::VEHICLE_SPAWN, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
				
				// send it back to the syncer of the vehicle so that he knows the id
				CVehiclePackets::VehicleConfirm vehicleConfirmPacket{};
				vehicleConfirmPacket.tempid = packet->tempid;
				vehicleConfirmPacket.vehicleid = packet->vehicleid;
				CNetwork::SendPacket(peer, CPacketsID::VEHICLE_CONFIRM, &vehicleConfirmPacket, sizeof vehicleConfirmPacket, ENET_PACKET_FLAG_RELIABLE);

				// create the network vehicle instance and add to the pool
				CVehicle* vehicle = new CVehicle(packet->vehicleid, packet->modelid, packet->pos, packet->rot);

				vehicle->m_pSyncer = player;
				vehicle->m_nPrimaryColor = packet->color1;
				vehicle->m_nSecondaryColor = packet->color2;
				vehicle->m_nCreatedBy = packet->createdBy;

				CVehicleManager::Add(vehicle);
			}
		};

		struct VehicleRemove
		{
			int vehicleid;

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CVehiclePackets::VehicleRemove* packet = (CVehiclePackets::VehicleRemove*)data;

				if (auto vehicle = CVehicleManager::GetVehicle(packet->vehicleid))
				{
					auto player = CPlayerManager::GetPlayer(peer);

					if (vehicle->m_pSyncer == player)
					{
						CNetwork::SendPacketToAll(CPacketsID::VEHICLE_REMOVE, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
						CVehicleManager::Remove(vehicle);
					}
					else
					{
						std::cout << "[Alert] " + player->GetName() + " tries to delete someone else's vehicle, possible hack or bug (please let us know)\n";
					}
				}
			}
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

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CVehiclePackets::VehicleIdleUpdate* packet = (CVehiclePackets::VehicleIdleUpdate*)data;
				
				if (auto vehicle = CVehicleManager::GetVehicle(packet->vehicleid))
				{
					auto player = CPlayerManager::GetPlayer(peer);

					if (vehicle->m_pSyncer == player)
					{
						vehicle->m_vecPosition = packet->pos;
						vehicle->m_vecRotation = packet->rot;
						CNetwork::SendPacketToAll(CPacketsID::VEHICLE_IDLE_UPDATE, packet, sizeof * packet, (ENetPacketFlag)0, peer);
					}
					else
					{
						std::cout << "[Alert] " + player->GetName() + " tries to sync someone else's vehicle, possible hack or bug (please let us know)\n";
					}
				}
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
			unsigned short miscComponentAngle; // hydra thrusters
			float planeGearState;
			unsigned char locked;

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				if (auto player = CPlayerManager::GetPlayer(peer))
				{
					CVehiclePackets::VehicleDriverUpdate* packet = (CVehiclePackets::VehicleDriverUpdate*)data;
					packet->playerid = player->m_iPlayerId;
					CNetwork::SendPacketToAll(CPacketsID::VEHICLE_DRIVER_UPDATE, packet, sizeof * packet, (ENetPacketFlag)0, peer);

					CVehicle* vehicle = CVehicleManager::GetVehicle(packet->vehicleid);

					if (vehicle)
					{
						vehicle->SetOccupant(0, player);

						vehicle->m_vecPosition = packet->pos;
						vehicle->m_vecRotation = packet->rot;

						vehicle->ReassignSyncer(player);
					}
				}
			}
		};

		struct VehicleEnter
		{
			int playerid;
			int vehicleid;
			unsigned char seatid : 3;
			unsigned char force : 1; // if true - put directly in vehicle (without any anim)
			unsigned char passenger : 1; 

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				if (auto player = CPlayerManager::GetPlayer(peer))
				{
					CVehiclePackets::VehicleEnter* packet = (CVehiclePackets::VehicleEnter*)data;
					packet->playerid = player->m_iPlayerId;
					CNetwork::SendPacketToAll(CPacketsID::VEHICLE_ENTER, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);

					if (auto vehicle = CVehicleManager::GetVehicle(packet->vehicleid))
					{
						if (packet->passenger)
							vehicle->SetOccupant(packet->seatid + 1, player);
						else
							vehicle->SetOccupant(0, player);
					}
				}
			}
		};

		struct VehicleExit
		{
			int playerid;
			bool force;

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				if (auto player = CPlayerManager::GetPlayer(peer))
				{
					CVehiclePackets::VehicleExit* packet = (CVehiclePackets::VehicleExit*)data;
					packet->playerid = player->m_iPlayerId;
					CNetwork::SendPacketToAll(CPacketsID::VEHICLE_EXIT, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);

					if (auto vehicle = CVehicleManager::GetVehicle(player->m_nVehicleId))
					{
						if (player->m_nSeatId >= 0)
							vehicle->SetOccupant(player->m_nSeatId, nullptr);
					}
				}
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
			unsigned char seatid;

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				if (auto player = CPlayerManager::GetPlayer(peer))
				{
					CVehiclePackets::VehiclePassengerUpdate* packet = (CVehiclePackets::VehiclePassengerUpdate*)data;
					packet->playerid = player->m_iPlayerId;
					CNetwork::SendPacketToAll(CPacketsID::VEHICLE_PASSENGER_UPDATE, packet, sizeof * packet, (ENetPacketFlag)0, peer);

					if (auto vehicle = CVehicleManager::GetVehicle(packet->vehicleid))
					{
						vehicle->SetOccupant(packet->seatid + 1, player);
						if (!vehicle->m_pPlayers[0]) // no driver
						{
							for (uint8_t i = 1; i < 8; i++)
							{
								if (auto catchedPlayer = vehicle->m_pPlayers[i])
								{
									vehicle->ReassignSyncer(catchedPlayer);
									break;
								}
							}
						}

					}
				}
			}
		};

		struct VehicleConfirm
		{
			unsigned char tempid;
			int vehicleid;
		};

		struct AssignVehicleSyncer
		{
			int vehicleid;
		};

		struct SetVehicleCreatedBy
		{
			int vehicleid;
			uint8_t createdBy;

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				if (auto player = CPlayerManager::GetPlayer(peer))
				{
					if (player->m_bIsHost)
					{
						SetVehicleCreatedBy* packet = (SetVehicleCreatedBy*)data;
						if (auto networkVehicle = CVehicleManager::GetVehicle(packet->vehicleid))
						{
							if (packet->createdBy >= 1 && packet->createdBy <= 4)
							{
								networkVehicle->m_nCreatedBy = packet->createdBy;
								CNetwork::SendPacketToAll(CPacketsID::SET_VEHICLE_CREATED_BY, data, sizeof(SetVehicleCreatedBy), ENET_PACKET_FLAG_RELIABLE, peer);
							}
						}
					}
				}
			}
		};

		~CVehiclePackets();
};

#endif