#pragma once

#ifndef _CPLAYER_H_
	#define _CPLAYER_H_
#include "../thirdparty-libraries/enet/enet.h"

#include "CVector.h"
#include "CNetwork.h"
#include "CPacket.h"
#include <string>

class CPlayer
{
	public:
		CPlayer(ENetPeer* peer, int playerid);

		std::string GetName();

		ENetPeer* m_pPeer;
		int m_iPlayerId;
		char m_Name[32 + 1] = { 0 };
		bool m_bIsHost;
		unsigned char m_nSeatId;
		int m_nVehicleId;
		bool m_bCorrectVersion = false;
		float m_afStats[14];
		unsigned int m_anModelKeys[10];
		unsigned int m_anTextureKeys[18];
		float m_fFatStat;
		float m_fMuscleStat;
		CVector m_vecWaypointPos{};
		struct {
			uint8_t bStatsModified : 1;
			uint8_t bClothesModified : 1;
			uint8_t bWaypointModified : 1;
		} m_ucSyncFlags;

		void RemoveFromVehicle();

		~CPlayer();
};

#endif