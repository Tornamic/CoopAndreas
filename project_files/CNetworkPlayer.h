#pragma once
class CNetworkPlayer
{
public:
	CPlayerPed* m_pPed = nullptr;

	CNetworkPlayer::CNetworkPlayer(int id, CVector position);
};

