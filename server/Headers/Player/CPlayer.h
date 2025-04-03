#if !defined(_COOPSERVER_PLAYER_PLAYER_H_) || !defined(_COOPSERVER_CPLAYER_H_)
#define _COOPSERVER_CPLAYER_H_
#define _COOPSERVER_PLAYER_PLAYER_H_

#include <string>
#include "../../include/enet/enet.h"

#include "../Vector/CVector.h"
#include "../Network/CNetwork.h"
#include "../Network/CPacket.h"

class CPlayer
{
	public:
		int m_iPlayerId;
		int m_nVehicleId;
		float m_afStats[14];
		float m_fFatStat;
		float m_fMuscleStat;
		unsigned int m_anModelKeys[10];
		unsigned int m_anTextureKeys[18];
		bool m_bCorrectVersion = false;
		bool m_bIsHost;
		char m_Name[32 + 1] = { 0 };
		unsigned char m_nSeatId;
		ENetPeer* m_pPeer;
		CVector m_vecWaypointPos{};
		
		struct {
			uint8_t bStatsModified : 1;
			uint8_t bClothesModified : 1;
			uint8_t bWaypointModified : 1;
		} m_ucSyncFlags;

		CPlayer(ENetPeer* peer, int playerid);

		std::string GetName();
		void RemoveFromVehicle();

		~CPlayer() { }
};

#endif