#include "stdafx.h"

CPlayer::CPlayer(ENetPeer* peer, int playerid)
{
	m_pPeer = peer;
	m_iPlayerId = playerid;
}