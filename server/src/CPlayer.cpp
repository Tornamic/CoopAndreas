
#include "../core/CPlayer.h"
#include "../core/CPlayerManager.h"

CPlayerPackets::PlayerOnFoot* m_lOnFoot;
char m_Name[32 + 1] = { 0 };
bool m_bIsHost = false;

CPlayer::CPlayer(ENetPeer* peer, int playerid)
{
	m_pPeer = peer;
	m_iPlayerId = playerid;
}