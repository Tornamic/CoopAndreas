#include "stdafx.h"

CPlayerPed* m_pPed = nullptr;
int m_iPlayerId;
int m_iBlipId;

// last sync data
CPackets::PlayerOnFoot* m_lOnFoot = nullptr;
CPackets::PlayerOnFoot* m_oOnFoot = nullptr;

const unsigned int CNetworkPlayer::m_pColours[] = { 0x6495EDFF,0xf0e68cFF,0x778899FF,0xFF1493FF,0xF4A460FF,0xEE82EEFF,0xFFD720FF,0x8b4513FF,0x4949A0FF,0x148b8bFF };

CNetworkPlayer::~CNetworkPlayer()
{
	if (m_pPed == nullptr) return;

	DWORD dwPedPtr = (DWORD)m_pPed;

	CRadar::ClearBlip(m_iBlipId);

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

	m_iBlipId = CRadar::SetEntityBlip(BLIP_CHAR, CPools::GetPedRef(player), 1, BLIP_DISPLAY_BLIP_ONLY);
	CRadar::ChangeBlipColour(m_iBlipId, CNetworkPlayer::m_pColours[id % 10]);
	CRadar::ChangeBlipScale(m_iBlipId, 3);
	CRadar::SetBlipFriendly(m_iBlipId, 1);

	m_pPed = player;
	m_iPlayerId = id;
}

int CNetworkPlayer::GetInternalId() // most used for CWorld::PlayerInFocus
{
	byte playerNumber = 0;

	for (; playerNumber < MAX_SERVER_PLAYERS + 2; playerNumber++)
	{
		if (m_pPed == CWorld::Players[playerNumber].m_pPed)
		{
			return playerNumber;
		}
	}

	return -1;
}