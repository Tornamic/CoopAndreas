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
		
		~CPlayer();
};

#endif