#include "stdafx.h"

CPlayerPed* m_pPed = nullptr;
int m_iPlayerId;

// last sync data
CPackets::PlayerOnFoot* m_lOnFoot = nullptr;

CNetworkPlayer::~CNetworkPlayer()
{
	if (m_pPed == nullptr) return;

	DWORD dwPedPtr = (DWORD)m_pPed;

	// fix destructor crash

	_asm mov esi, dwPedPtr
	_asm mov eax, [esi + 1152]
	_asm mov dword ptr[eax + 76], 0

	// call destroy method
	_asm mov ecx, dwPedPtr
	_asm mov ebx, [ecx]; vtable
	_asm push 1
	_asm call[ebx]; destroy
}

CNetworkPlayer::CNetworkPlayer(int id, CVector position)
{
	
	// load CJ (0) model
	CStreaming::RequestModel(0, eStreamingFlags::GAME_REQUIRED | eStreamingFlags::PRIORITY_REQUEST);
	CStreaming::LoadAllRequestedModels(false);

	// wait until model loaded
	while (CStreaming::ms_aInfoForModel[0].m_nLoadState != LOADSTATE_LOADED)
		Sleep(5);

	// get player info
	CPlayerInfo* info = &CWorld::Players[id + 2];

	// create new player ped
	CPlayerPed* player = new CPlayerPed(id + 2, 0);

	// copy player ped instance to info
	info->m_pPed = player;

	// set player state to PLAYING
	info->m_nPlayerState = ePlayerState::PLAYERSTATE_PLAYING;

	// set ped type to PLAYER1, dont use any other !!!
	player->m_nPedType = ePedType::PED_TYPE_PLAYER1;

	CWorld::Add(player);
	
	player->SetPosn(FindPlayerCoors(0));
	player->SetModelIndex(0);
	player->SetOrientation(0.0f, 0.0f, 0.0f);

	//m_pPed->m_bStreamingDontDelete = 1;

	m_pPed = player;
	m_iPlayerId = id;
}

