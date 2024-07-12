#pragma once
class CNetworkPlayer
{
public:
	CPlayerPed* m_pPed = nullptr;
	int m_iPlayerId;

	// last sync data
	CPackets::PlayerOnFoot* m_lOnFoot = nullptr;
	CPackets::PlayerOnFoot* m_oOnFoot = nullptr;
	
	CNetworkPlayer::~CNetworkPlayer();
	CNetworkPlayer::CNetworkPlayer(int id, CVector position);
};

