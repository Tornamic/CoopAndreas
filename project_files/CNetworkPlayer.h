#pragma once
class CNetworkPlayer
{
public:
	CPlayerPed* m_pPed = nullptr;
	int m_nPlayerId;

	// last sync data
	CPackets::PlayerOnFoot* m_lOnFoot = nullptr;
	CPackets::PlayerOnFoot* m_oOnFoot = nullptr;
	
	signed short m_oShockButtonL;
	signed short m_lShockButtonL;
	
	static const unsigned int m_pColours[];

	CVector* m_vecWaypointPos = nullptr;
	bool m_bWaypointPlaced = false;

	char m_name[32 + 1] = { 0 };

	CVector m_aPassengerAim{};

	~CNetworkPlayer();
	CNetworkPlayer(int id, CVector position);

	CControllerState m_oldControllerState{};
	CControllerState m_newControllerState{};

	ÑCompressedControllerState m_compressedControllerState{};

	bool m_bStreamed = false;

	void CreatePed(int id, CVector position);

	void DestroyPed();

	int GetInternalId();
	char* GetName();
	bool IsStreamed();
	void StreamIn(const CVector& position);
	void StreamOut();
};

