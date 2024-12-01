#pragma once

#ifndef _CPLAYERMANAGER_H_
	#define _CPLAYERMANAGER_H_
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>

#include "CControllerState.h"
#include "CPlayer.h"

class CPlayerManager
{
	public:
		CPlayerManager();
		
		static std::vector<CPlayer*> m_pPlayers;
		static void Add(CPlayer* player);
		static void Remove(CPlayer* player);
		static CPlayer* GetPlayer(int playerid);
		static CPlayer* GetPlayer(ENetPeer* peer);
		static int GetFreeID();
		static CPlayer* GetHost();
		static void AssignHostToFirstPlayer();
		
		~CPlayerManager();
};


class CPlayerPackets
{
	public:
		CPlayerPackets();

		struct PlayerConnected
		{
			int id;
			char name[32 + 1];
		};

		struct PlayerDisconnected
		{
			int id;
			unsigned char reason;
		};

		#pragma pack(1)
		struct PlayerOnFoot
		{	
			int id;
			CVector position;
			CVector	velocity;
			float rotation;
			unsigned char health;
			unsigned char armour;
			unsigned char weapon;
			unsigned short ammo;
			bool ducking;
			float aimX;
			float aimY;
			bool hasJetpack = false;

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				// create packet
				CPlayerPackets::PlayerOnFoot* packet = (CPlayerPackets::PlayerOnFoot*)data;

				// set packet`s playerid, cuz incoming packet has id = 0
				packet->id = CPlayerManager::GetPlayer(peer)->m_iPlayerId;


				CNetwork::SendPacketToAll(CPacketsID::PLAYER_ONFOOT, packet, sizeof *packet, ENET_PACKET_FLAG_UNSEQUENCED, peer);
			}
		};

		#pragma pack(1)
		struct PlayerBulletShot
		{
			int playerid;
			int targetid;
			CVector startPos;
			CVector endPos;
			unsigned char colPoint[44]; // padding
			int incrementalHit;
			unsigned char entityType;
	
			static void Handle(ENetPeer* peer, void* data, int size)
			{
					// create packet
					CPlayerPackets::PlayerBulletShot* packet = (CPlayerPackets::PlayerBulletShot*)data;
						
							// set packet`s playerid, cuz incoming packet has id = 0
						packet->playerid = CPlayerManager::GetPlayer(peer)->m_iPlayerId;
			
					CNetwork::SendPacketToAll(CPacketsID::PLAYER_BULLET_SHOT, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED, peer);
				}
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
	
			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CPlayerPackets::PlayerPlaceWaypoint* packet = (CPlayerPackets::PlayerPlaceWaypoint*)data;
				packet->playerid = CPlayerManager::GetPlayer(peer)->m_iPlayerId;
				CNetwork::SendPacketToAll(CPacketsID::PLAYER_PLACE_WAYPOINT, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
			}
		};

		struct PlayerGetName
		{
			int playerid;
			char name[32 + 1];
	
			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CPlayerPackets::PlayerGetName* packet = (CPlayerPackets::PlayerGetName*)data;
				CPlayer* player = CPlayerManager::GetPlayer(peer);
				packet->playerid = player->m_iPlayerId;
				strcpy(player->m_Name, packet->name);
				printf("player %d now also know as %s\n", packet->playerid, packet->name);
				CNetwork::SendPacketToAll(CPacketsID::PLAYER_GET_NAME, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
			}
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

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CPlayerPackets::AddExplosion* packet = (CPlayerPackets::AddExplosion*)data;
				CNetwork::SendPacketToAll(CPacketsID::ADD_EXPLOSION, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
			}
		};

		struct PlayerChatMessage
		{
			int playerid;
			char message[128 + 1];

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CPlayerPackets::PlayerChatMessage* packet = (CPlayerPackets::PlayerChatMessage*)data;
				packet->playerid = CPlayerManager::GetPlayer(peer)->m_iPlayerId;
				CNetwork::SendPacketToAll(CPacketsID::PLAYER_CHAT_MESSAGE, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
			}
		};

		struct PlayerKeySync
		{
			int playerid;
			CCompressedControllerState newState;

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CPlayerPackets::PlayerKeySync* packet = (CPlayerPackets::PlayerKeySync*)data;
				packet->playerid = CPlayerManager::GetPlayer(peer)->m_iPlayerId;
				CNetwork::SendPacketToAll(CPacketsID::PLAYER_KEY_SYNC, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
			}
		};

		~CPlayerPackets();
};


#endif