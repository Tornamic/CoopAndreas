#pragma once
class CPlayer
{
public:
	ENetPeer* m_pPeer;
	int m_iPlayerId;

	CPlayer(ENetPeer* peer, int playerid);

};

