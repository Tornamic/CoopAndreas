#pragma once
class CPlayer
{
public:
	ENetPeer* m_pPeer;
	int m_iPlayerId;
	char m_Name[32 + 1] = { 0 };

	CPlayer(ENetPeer* peer, int playerid);

};

