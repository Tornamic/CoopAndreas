#include "stdafx.h"

CPlayerPed* m_pPed = nullptr;

CNetworkPlayer::CNetworkPlayer(int id, CVector position)
{
	// load CJ (0) model
	CStreaming::RequestModel(0, eStreamingFlags::GAME_REQUIRED | eStreamingFlags::PRIORITY_REQUEST);
	CStreaming::LoadAllRequestedModels(false);

	// get player info
	CPlayerInfo* info = &CWorld::Players[id + 2];

	// create new player ped
	CPlayerPed* player = new CPlayerPed(id + 2, 0);

	// copy player ped instance to info
	info->m_pPed = player;

	// set player state to PLAYING
	info->m_nPlayerState = ePlayerState::PLAYERSTATE_PLAYING;

	// set ped type to PLAYER1, dont use any other !!!
	player->m_nPedType = ePedType::PED_TYPE_PLAYER2;

	CWorld::Add(player);
	
	player->SetModelIndex(0);
	player->SetPosn(position);
	player->SetOrientation(0.0f, 0.0f, 0.0f);

	m_pPed = player;
}

