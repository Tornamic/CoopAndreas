#pragma once
class CNetworkPlayer
{
public:
	CPlayerPed* m_pPed = nullptr;
	int m_iPlayerId;
	int m_iBlipId;

	// last sync data
	CPackets::PlayerOnFoot* m_lOnFoot = nullptr;
	CPackets::PlayerOnFoot* m_oOnFoot = nullptr;
	
	CNetworkPlayer::~CNetworkPlayer();
	CNetworkPlayer::CNetworkPlayer(int id, CVector position);

	static const unsigned int m_pColours[];
};

