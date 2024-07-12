#pragma once
class CPlayer
{
public:
	ENetPeer* m_pPeer;
	int m_iPlayerId;

	CPackets::PlayerOnFoot* m_lOnFoot;

	CPlayer(ENetPeer* peer, int playerid);
	
	CVector GetPos();
};

