#if !defined(_COOPSERVER_PLAYER_PLAYERMANAGER_H_) || !defined(_COOPSERVER_CPLAYERMANAGER_H_)
#define _COOPSERVER_PLAYER_PLAYERMANAGER_H_
#define _COOPSERVER_CPLAYERMANAGER_H_

#include "CPlayer.h"
#include "../InputControllers/CControllerState.h"
#include "../Network/NetworkEntityType.h"

class CPlayerManager
{
	public:
		static std::vector<CPlayer*> m_pPlayers;

		CPlayerManager() { }
		
		static void Add(CPlayer* player);
		static void Remove(CPlayer* player);
		static CPlayer* GetPlayer(int playerid);
		static CPlayer* GetPlayer(ENetPeer* peer);
		static int GetFreeID();
		static CPlayer* GetHost();
		static void AssignHostToFirstPlayer();
		
		~CPlayerManager() { }
};



#endif