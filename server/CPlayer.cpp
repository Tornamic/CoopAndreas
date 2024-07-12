#include "stdafx.h"

CPackets::PlayerOnFoot* m_lOnFoot;


CPlayer::CPlayer(ENetPeer* peer, int playerid)
{
	m_pPeer = peer;
	m_iPlayerId = playerid;
}

CVector CPlayer::GetPos() 
{ 
	if (m_lOnFoot == nullptr)
		return CVector(0, 0, 0);

	return m_lOnFoot->position;
}