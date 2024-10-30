#include "stdafx.h"

CPlayerPed* m_pPed = nullptr;
int m_iPlayerId;

// last sync data
CPackets::PlayerOnFoot* m_lOnFoot = nullptr;
CPackets::PlayerOnFoot* m_oOnFoot = nullptr;

signed short m_oShockButtonL = 0;
signed short m_lShockButtonL = 0;

const unsigned int CNetworkPlayer::m_pColours[] = { 0x6495EDFF,0xf0e68cFF,0x778899FF,0xFF1493FF,0xF4A460FF,0xEE82EEFF,0xFFD720FF,0x8b4513FF,0x4949A0FF,0x148b8bFF };

CVector* m_vecWaypointPos = nullptr;
bool m_bWaypointPlaced = false;

char m_name[32 + 1] = { 0 };

CNetworkPlayer::~CNetworkPlayer()
{
	DestroyPed();
}

CNetworkPlayer::CNetworkPlayer(int id, CVector position)
{
	m_nPlayerId = id;

	m_lOnFoot = m_oOnFoot = new CPackets::PlayerOnFoot();
}

void CNetworkPlayer::CreatePed(int id, CVector position)
{
	DestroyPed(); // destroy ped if present

	unsigned int actorId = 0;
	int playerId = id + 2;

	plugin::Command<Commands::CREATE_PLAYER>(playerId, position.x, position.y, position.z, &actorId);
	plugin::Command<Commands::GET_PLAYER_CHAR>(playerId, &actorId);

	m_pPed = (CPlayerPed*)CPools::GetPed(actorId);

	m_pPed->SetOrientation(0.0f, 0.0f, 0.0f);

	// set player immunies, he now dont cares about pain
	Command<Commands::SET_CHAR_PROOFS>(actorId, 0, 1, 1, 0, 0);
}

void CNetworkPlayer::DestroyPed()
{
	if (m_pPed == nullptr) return;

	if (m_pPed->m_pVehicle != nullptr)
	{
		plugin::Command<Commands::WARP_CHAR_FROM_CAR_TO_COORD>(CPools::GetPedRef(m_pPed), 0.f, 0.f, 0.f);
	}

	// call destroy method
	_asm mov ecx, m_pPed
	_asm mov ebx, [ecx] // vtable
	_asm push 1
	_asm call[ebx] // destroy
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

char* CNetworkPlayer::GetName()
{
	char* buffer = new char[32 + 1];

	if (m_name[0] == '\0')
		sprintf(buffer, "player %d", m_nPlayerId);
	else
		strcpy(buffer, m_name);

	return buffer;
}

bool CNetworkPlayer::IsStreamed()
{
	return m_bStreamed;
}

void CNetworkPlayer::StreamIn(const CVector& position)
{
	m_bStreamed = true;
	CreatePed(m_nPlayerId, position);
}

void CNetworkPlayer::StreamOut()
{
	m_bStreamed = false;
	DestroyPed();
}