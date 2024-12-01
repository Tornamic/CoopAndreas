#pragma once
class CNetworkPlayer
{
public:
	CPlayerPed* m_pPed = nullptr;
	int m_iPlayerId;

	// last sync data
	CPackets::PlayerOnFoot* m_lOnFoot = nullptr;
	CPackets::PlayerOnFoot* m_oOnFoot = nullptr;
	
	signed short m_oShockButtonL;
	signed short m_lShockButtonL;
	
	static const unsigned int m_pColours[];

	CVector* m_vecWaypointPos = nullptr;
	bool m_bWaypointPlaced = false;

	char m_Name[32 + 1] = { 0 };

	CVector m_aPassengerAim{};

	CNetworkPlayer::~CNetworkPlayer();
	CNetworkPlayer::CNetworkPlayer(int id, CVector position);

	CControllerState m_oldControllerState{};
	CControllerState m_newControllerState{};

	�CompressedControllerState m_compressedControllerState{};

	void CreatePed(int id, CVector position);

	int GetInternalId();
	char* GetName();
};

