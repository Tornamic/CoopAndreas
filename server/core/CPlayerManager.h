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
#include "NetworkEntityType.h"
#include "PlayerDisconnectReason.h"

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
		bool isAlreadyConnected; // prevents spam in the chat when connecting by distinguishing already connected players from newly joined ones
		char name[32 + 1];
		uint32_t version;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (CPlayerManager::GetPlayer(peer))
			{
				return;
			}

			CNetwork::HandlePlayerConnected(peer, data, size);
		}
	};

	struct PlayerDisconnected
	{
		int id;
		ePlayerDisconnectReason reason;
		uint32_t version;
	};

#pragma pack(1)
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

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				// create packet
				CPlayerPackets::PlayerOnFoot* packet = (CPlayerPackets::PlayerOnFoot*)data;

				// set packet`s playerid, cuz incoming packet has id = 0
				packet->id = player->m_iPlayerId;

				bool isValidWeapon = (packet->weapon >= 0 && packet->weapon <= 18) || (packet->weapon >= 22 && packet->weapon <= 46);
				if (!isValidWeapon)
				{
					packet->weapon = 0;
					packet->ammo = 0;
				}

				if (packet->fightingStyle < 4 || packet->fightingStyle > 16)
				{
					packet->fightingStyle = 4;
				}

				if (packet->velocity.x > 10.0f || packet->velocity.y > 10.0f || packet->velocity.z > 10.0f)
				{
					packet->velocity = CVector(0.0f, 0.0f, 0.0f);
				}

				if (player->m_nVehicleId >= 0)
				{
					player->RemoveFromVehicle();
				}

				CNetwork::SendPacketToAll(CPacketsID::PLAYER_ONFOOT, packet, sizeof * packet, (ENetPacketFlag)0, peer);
			}
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

			CNetwork::SendPacketToAll(CPacketsID::PLAYER_BULLET_SHOT, packet, sizeof * packet, (ENetPacketFlag)0, peer);
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
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				CPlayerPackets::PlayerPlaceWaypoint* packet = (CPlayerPackets::PlayerPlaceWaypoint*)data;
				packet->playerid = player->m_iPlayerId;
				packet->position.x = std::clamp(packet->position.x, -3000.0f, 3000.0f);
				packet->position.y = std::clamp(packet->position.y, -3000.0f, 3000.0f);
				player->m_ucSyncFlags.bWaypointModified = packet->place != 0;
				player->m_vecWaypointPos = packet->position;
				CNetwork::SendPacketToAll(CPacketsID::PLAYER_PLACE_WAYPOINT, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
			}
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
		bool usesSound;
		float cameraShake;
		bool isVisible;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			CPlayerPackets::AddExplosion* packet = (CPlayerPackets::AddExplosion*)data;
			CNetwork::SendPacketToAll(CPacketsID::ADD_EXPLOSION, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
		}
	};

	struct PlayerChatMessage
	{
		int playerid;
		wchar_t message[128 + 1];

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			CPlayerPackets::PlayerChatMessage* packet = (CPlayerPackets::PlayerChatMessage*)data;
			packet->playerid = CPlayerManager::GetPlayer(peer)->m_iPlayerId;
			packet->message[128] = 0;
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
			CNetwork::SendPacketToAll(CPacketsID::PLAYER_KEY_SYNC, packet, sizeof * packet, (ENetPacketFlag)0, peer);
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
		unsigned char weaponCameraMode;
		float cameraFov;
		CVector front;
		CVector	source;
		CVector	up;
		float lookPitch;
		float orientation;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			CPlayerPackets::PlayerAimSync* packet = (CPlayerPackets::PlayerAimSync*)data;
			packet->playerid = CPlayerManager::GetPlayer(peer)->m_iPlayerId;
			CNetwork::SendPacketToAll(CPacketsID::PLAYER_AIM_SYNC, packet, sizeof * packet, (ENetPacketFlag)0, peer);
		}
	};

	struct PlayerStats
	{
		int playerid;
		float stats[14];

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				CPlayerPackets::PlayerStats* packet = (CPlayerPackets::PlayerStats*)data;
				packet->playerid = player->m_iPlayerId;
				CNetwork::SendPacketToAll(CPacketsID::PLAYER_STATS, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);

				memcpy(player->m_afStats, packet->stats, sizeof(packet->stats));
				player->m_ucSyncFlags.bStatsModified = true;
			}
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
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				CPlayerPackets::RebuildPlayer* packet = (CPlayerPackets::RebuildPlayer*)data;
				packet->playerid = player->m_iPlayerId;
				CNetwork::SendPacketToAll(CPacketsID::REBUILD_PLAYER, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);

				memcpy(player->m_anModelKeys, packet->m_anModelKeys, sizeof(player->m_anModelKeys));
				memcpy(player->m_anTextureKeys, packet->m_anTextureKeys, sizeof(player->m_anTextureKeys));
				player->m_fFatStat = packet->m_fFatStat;
				player->m_fMuscleStat = packet->m_fMuscleStat;
				player->m_ucSyncFlags.bClothesModified = true;
			}
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

	struct StartCutscene
	{
		char name[8];
		uint8_t currArea; // AKA interior

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				if (player->m_bIsHost)
				{
					CPlayerPackets::StartCutscene* packet = (CPlayerPackets::StartCutscene*)data;
					CNetwork::SendPacketToAll(CPacketsID::START_CUTSCENE, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
				}
			}
		}
	};

	struct OpCodeSync
	{
		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				/*if (player->m_bIsHost)
				{*/
					CNetwork::SendPacketToAll(CPacketsID::OPCODE_SYNC, data, size, ENET_PACKET_FLAG_RELIABLE, peer);
				//}
			}
		}
	};

	struct SkipCutscene
	{
		int playerid;
		int votes; // temporary unused

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				CNetwork::SendPacketToAll(CPacketsID::SKIP_CUTSCENE, data, size, ENET_PACKET_FLAG_RELIABLE, peer);
			}
		}
	};

	struct OnMissionFlagSync
	{
		uint8_t bOnMission : 1;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				if (player->m_bIsHost)
				{
					CNetwork::SendPacketToAll(CPacketsID::ON_MISSION_FLAG_SYNC, data, sizeof(OnMissionFlagSync), ENET_PACKET_FLAG_RELIABLE, peer);
				}
			}
		}
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

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				if (player->m_bIsHost)
				{
					UpdateEntityBlip* packet = (UpdateEntityBlip*)data;

					if (auto targetPlayer = CPlayerManager::GetPlayer(packet->playerid))
					{
						CNetwork::SendPacket(targetPlayer->m_pPeer, CPacketsID::UPDATE_ENTITY_BLIP, data, sizeof(UpdateEntityBlip), ENET_PACKET_FLAG_RELIABLE);
					}
				}
			}
		}
	};

	struct RemoveEntityBlip
	{
		int playerid;
		eNetworkEntityType entityType;
		int entityId;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				if (player->m_bIsHost)
				{
					RemoveEntityBlip* packet = (RemoveEntityBlip*)data;

					if (auto targetPlayer = CPlayerManager::GetPlayer(packet->playerid))
					{
						CNetwork::SendPacket(targetPlayer->m_pPeer, CPacketsID::REMOVE_ENTITY_BLIP, data, sizeof(RemoveEntityBlip), ENET_PACKET_FLAG_RELIABLE);
					}
				}
			}
		}
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

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				if (player->m_bIsHost)
				{
					AddMessageGXT* packet = (AddMessageGXT*)data;

					if (auto targetPlayer = CPlayerManager::GetPlayer(packet->playerid))
					{
						CNetwork::SendPacket(targetPlayer->m_pPeer, CPacketsID::ADD_MESSAGE_GXT, data, sizeof(AddMessageGXT), ENET_PACKET_FLAG_RELIABLE);
					}
				}
			}
		}
	};

	struct RemoveMessageGXT
	{
		int playerid;
		char gxt[8];

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				if (player->m_bIsHost)
				{
					RemoveMessageGXT* packet = (RemoveMessageGXT*)data;

					if (auto targetPlayer = CPlayerManager::GetPlayer(packet->playerid))
					{
						CNetwork::SendPacket(targetPlayer->m_pPeer, CPacketsID::REMOVE_MESSAGE_GXT, data, sizeof(RemoveMessageGXT), ENET_PACKET_FLAG_RELIABLE);
					}
				}
			}
		}
	};

	struct ClearEntityBlips
	{
		int playerid;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				if (player->m_bIsHost)
				{
					ClearEntityBlips* packet = (ClearEntityBlips*)data;

					if (auto targetPlayer = CPlayerManager::GetPlayer(packet->playerid))
					{
						CNetwork::SendPacket(targetPlayer->m_pPeer, CPacketsID::CLEAR_ENTITY_BLIPS, data, sizeof(ClearEntityBlips), ENET_PACKET_FLAG_RELIABLE);
					}
				}
			}
		}
	};

	struct PlayMissionAudio
	{
		uint8_t slotid;
		int audioid;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				if (player->m_bIsHost)
				{
					PlayMissionAudio* packet = (PlayMissionAudio*)data;
					CNetwork::SendPacketToAll(CPacketsID::PLAY_MISSION_AUDIO, packet, sizeof(PlayMissionAudio), ENET_PACKET_FLAG_RELIABLE, peer);
				}
			}
		}
	};

	struct UpdateCheckpoint
	{
		int playerid;
		CVector position;
		CVector radius;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				if (player->m_bIsHost)
				{
					UpdateCheckpoint* packet = (UpdateCheckpoint*)data;

					if (auto targetPlayer = CPlayerManager::GetPlayer(packet->playerid))
					{
						CNetwork::SendPacket(targetPlayer->m_pPeer, CPacketsID::UPDATE_CHECKPOINT, data, sizeof(UpdateCheckpoint), ENET_PACKET_FLAG_RELIABLE);
					}
				}
			}
		}
	};

	struct RemoveCheckpoint
	{
		int playerid;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				if (player->m_bIsHost)
				{
					RemoveCheckpoint* packet = (RemoveCheckpoint*)data;

					if (auto targetPlayer = CPlayerManager::GetPlayer(packet->playerid))
					{
						CNetwork::SendPacket(targetPlayer->m_pPeer, CPacketsID::REMOVE_CHECKPOINT, data, sizeof(RemoveCheckpoint), ENET_PACKET_FLAG_RELIABLE);
					}
				}
			}
		}
	};

	struct EnExSync
	{
		static inline std::vector<uint8_t> ms_vLastData;
		static inline CPlayer* ms_pLastPlayerOwner;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				if (player->m_bIsHost)
				{
					ms_vLastData.assign((uint8_t*)data, (uint8_t*)data + size);
					ms_pLastPlayerOwner = player;

					CNetwork::SendPacketToAll(CPacketsID::ENEX_SYNC, data, size, ENET_PACKET_FLAG_RELIABLE, peer);
				}
			}
		}
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

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				if (player->m_bIsHost)
				{
					CNetwork::SendPacketToAll(CPacketsID::CREATE_STATIC_BLIP, data, sizeof(CreateStaticBlip), ENET_PACKET_FLAG_RELIABLE, peer);
				}
			}
		}
	};

	struct SetPlayerTask
	{
		int playerid;
		int taskType;
		CVector position;
		float rotation;
		bool toggle;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				SetPlayerTask* packet = (SetPlayerTask*)data;
				packet->playerid = player->m_iPlayerId;
				CNetwork::SendPacketToAll(CPacketsID::SET_PLAYER_TASK, packet, sizeof(SetPlayerTask), ENET_PACKET_FLAG_RELIABLE, peer);
			}
		}
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

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				PedSay* packet = (PedSay*)data;

				if (packet->isPlayer)
				{
					packet->entityid = player->m_iPlayerId;
				}

				CNetwork::SendPacketToAll(CPacketsID::PED_SAY, packet, sizeof(PedSay), ENET_PACKET_FLAG_RELIABLE, peer);
			}
		}
	};

	struct AddProjectile
	{
		uint8_t creatorType;
		int creatorId;
		uint8_t projectileType; // eWeaponType
		CVector origin;
		float force;
		CVector dir;
		uint8_t targetType;
		int targetId;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				CNetwork::SendPacketToAll(CPacketsID::ADD_PROJECTILE, data, sizeof(AddProjectile), ENET_PACKET_FLAG_RELIABLE, peer);
			}
		}
	};

	struct TagUpdate
	{
		int16_t pos_x;
		int16_t pos_y;
		int16_t pos_z;
		uint8_t alpha;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				CNetwork::SendPacketToAll(CPacketsID::TAG_UPDATE, data, sizeof(TagUpdate), ENET_PACKET_FLAG_RELIABLE, peer);
			}
		}
	};

	struct UpdateAllTags
	{
		TagUpdate tags[150];

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			if (auto player = CPlayerManager::GetPlayer(peer))
			{
				if (player->m_bIsHost)
				{
					CNetwork::SendPacketToAll(CPacketsID::UPDATE_ALL_TAGS, data, sizeof(UpdateAllTags), ENET_PACKET_FLAG_RELIABLE, peer);
				}
			}
		}
	};
};
#endif