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
		#pragma pack(1)
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
			bool hasJetpack = false;
			char fightingStyle;

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
				printf("[Game] : Player %d now also know as '%s'\n", packet->playerid, packet->name);
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

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				if (!CPlayerManager::GetPlayer(peer)->m_bIsHost)
					return;

				CPlayerPackets::GameWeatherTime* packet = (CPlayerPackets::GameWeatherTime*)data;
				CNetwork::SendPacketToAll(CPacketsID::GAME_WEATHER_TIME, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
			}
		};

		struct PlayerAimSync
		{
			int playerid;
			unsigned char cameraMode;
			float cameraFov;
			CVector front;
			CVector	source;
			CVector	up;
			float moveHeading;
			float aimY;
			float aimZ;

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CPlayerPackets::PlayerAimSync* packet = (CPlayerPackets::PlayerAimSync*)data;
				packet->playerid = CPlayerManager::GetPlayer(peer)->m_iPlayerId;
				CNetwork::SendPacketToAll(CPacketsID::PLAYER_AIM_SYNC, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED, peer);
			}
		};

		struct PlayerStats
		{
			int playerid;
			float stats[14];

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CPlayerPackets::PlayerStats* packet = (CPlayerPackets::PlayerStats*)data;
				packet->playerid = CPlayerManager::GetPlayer(peer)->m_iPlayerId;
				CNetwork::SendPacketToAll(CPacketsID::PLAYER_STATS, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
			}
		}; 
		
		struct RebuildPlayer
		{
			int playerid;
			// CPedClothesDesc inlined
			unsigned int m_anModelKeys[10];
			unsigned int m_anTextureKeys[18];
			float m_fFatStat;
			float m_fMuscleStat;

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CPlayerPackets::RebuildPlayer* packet = (CPlayerPackets::RebuildPlayer*)data;
				packet->playerid = CPlayerManager::GetPlayer(peer)->m_iPlayerId;
				CNetwork::SendPacketToAll(CPacketsID::REBUILD_PLAYER, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
			}
		};

		struct RespawnPlayer
		{
			int playerid;

			static void Handle(ENetPeer* peer, void* data, int size)
			{
				CPlayerPackets::RespawnPlayer* packet = (CPlayerPackets::RespawnPlayer*)data;
				packet->playerid = CPlayerManager::GetPlayer(peer)->m_iPlayerId;
				CNetwork::SendPacketToAll(CPacketsID::RESPAWN_PLAYER, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
			}
		};

		~CPlayerPackets();
};


#endif