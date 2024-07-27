#pragma once

enum CPacketsID : unsigned short
{
	PLAYER_CONNECTED,
	PLAYER_DISCONNECTED,
	PLAYER_ONFOOT,
	PLAYER_BULLET_SHOT,
	PLAYER_HANDSHAKE
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

		char CAMERA_AIM[48]; // padding for CAMERA_AIM struct
		unsigned char cameraMode;

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
};