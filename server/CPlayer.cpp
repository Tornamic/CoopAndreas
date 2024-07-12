#include "stdafx.h"

CPackets::PlayerOnFoot* m_lOnFoot;


CPlayer::CPlayer(ENetPeer* peer, int playerid)
{
	m_pPeer = peer;
	m_iPlayerId = playerid;
}