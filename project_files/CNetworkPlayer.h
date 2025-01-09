#pragma once
class CNetworkPlayer
{
public:
	CPlayerPed* m_pPed = nullptr;
	int m_iPlayerId;

	CPackets::PlayerOnFoot* m_lOnFoot = nullptr;
	
	signed short m_oShockButtonL;
	signed short m_lShockButtonL;

	CVector* m_vecWaypointPos = nullptr;
	bool m_bWaypointPlaced = false;

	char m_Name[32 + 1] = { 0 };

	CControllerState m_oldControllerState{};
	CControllerState m_newControllerState{};
	CCompressedControllerState m_compressedControllerState{};

	CPackets::PlayerAimSync m_aimSyncData;

	CNetworkPlayerStats m_stats{};
	CPedClothesDesc m_pPedClothesDesc{};

	CNetworkPlayer::~CNetworkPlayer();
	CNetworkPlayer::CNetworkPlayer(int id, CVector position);

	void CreatePed(int id, CVector position);
	void DestroyPed();
	void Respawn();
	int GetInternalId();
	char* GetName();
	char GetWeaponSkill(eWeaponType weaponType);
	void RemoveFromVehicle(CVehicle* vehicle);
	void WarpIntoVehiclePassenger(CVehicle* vehicle, int seatid);
	void EnterVehiclePassenger(CVehicle* vehicle, int seatid);
};

