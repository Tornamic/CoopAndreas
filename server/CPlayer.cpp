#include "stdafx.h"

CPackets::PlayerOnFoot* m_lOnFoot;
char m_Name[32 + 1] = { 0 };

CPlayer::CPlayer(ENetPeer* peer, int playerid)
{
	m_pPeer = peer;
	m_iPlayerId = playerid;
}