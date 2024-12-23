
#include "../core/CPlayer.h"
#include "../core/CPlayerManager.h"

CPlayer::CPlayer(ENetPeer* peer, int playerid)
{
	m_pPeer = peer;
	m_iPlayerId = playerid;
}

std::string CPlayer::GetName()
{
	if (m_Name[0])
		return std::string(m_Name);
	else
		return "player " + std::to_string(m_iPlayerId);
}