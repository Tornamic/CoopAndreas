#pragma once

enum CPacketsID : unsigned short
{
	PLAYER_CONNECTED,
	PLAYER_DISCONNECTED,
	PLAYER_ONFOOT,
	PLAYER_BULLET_SHOT,
	PLAYER_HANDSHAKE,
	PLAYER_PLACE_WAYPOINT,
	PLAYER_GET_NAME
};

class CPackets
{
public:
	struct PlayerConnected
	{
		int id;
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
		CControllerState controllerState;
		unsigned char health;
		unsigned char armour;
		unsigned char weapon;
		unsigned short ammo;
		bool ducking;
		float aimX;
		float aimY;

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			// create packet
			CPackets::PlayerOnFoot* packet = (CPackets::PlayerOnFoot*)data;

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

		static void Handle(ENetPeer* peer, void* data, int size)
		{
			// create packet
			CPackets::PlayerBulletShot* packet = (CPackets::PlayerBulletShot*)data;
			
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
			CPackets::PlayerPlaceWaypoint* packet = (CPackets::PlayerPlaceWaypoint*)data;
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
			CPackets::PlayerGetName* packet = (CPackets::PlayerGetName*)data;
			CPlayer* player = CPlayerManager::GetPlayer(peer);
			packet->playerid = player->m_iPlayerId;
			strcpy_s(player->m_Name, packet->name);
			printf("player %d now also know as %s\n", packet->playerid, packet->name);
			CNetwork::SendPacketToAll(CPacketsID::PLAYER_GET_NAME, packet, sizeof * packet, ENET_PACKET_FLAG_RELIABLE, peer);
		}
	};
};