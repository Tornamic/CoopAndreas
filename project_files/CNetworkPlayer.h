#pragma once
class CNetworkPlayer
{
public:
	CPlayerPed* m_pPed = nullptr;
	int m_iPlayerId;

	// last sync data
	CPackets::PlayerOnFoot* m_lOnFoot = nullptr;

	CNetworkPlayer::CNetworkPlayer(int id, CVector position);
};

