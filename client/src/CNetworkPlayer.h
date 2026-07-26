#pragma once

class CNetworkPlayer
{
public:
	CPlayerPed* m_pPed = nullptr;
	int m_iPlayerId;

	Packets::Players::OnFootUpdate m_onFootSnapshotInterpolated{};
	
	uint32_t m_startedInterpolatingCameraAt = 0;
	Packets::Players::PlayerCameraSync m_cameraSnapshotOld{};
	Packets::Players::PlayerCameraSync m_cameraSnapshot{};
	
	signed short m_oShockButtonL;
	signed short m_lShockButtonL;

	Packets::Players::PlayerPlaceWaypoint m_waypointState{};

	char m_Name[32 + 1] = { 0 };

	CControllerState m_oldControllerState{};
	CControllerState m_newControllerState{};

	CNetworkPlayerStats m_stats{};
	CPedClothesDesc m_pPedClothesDesc{};
	bool m_bHasBeenConnectedBeforeMe = false;

	bool m_bExtrapolating = false;

	bool m_bRequestedDuckTask = false;

	bool m_bIsHost = false;

	~CNetworkPlayer();
	CNetworkPlayer(int id, CVector position);

	void CreatePed(int id, CVector position);
	void DestroyPed();
	void Respawn();
	int GetInternalId();
	std::string GetName();
	char GetWeaponSkill(eWeaponType weaponType);
	void WarpIntoVehicleDriver(CVehicle* vehicle);
	void RemoveFromVehicle(CVehicle* vehicle);
	void WarpIntoVehiclePassenger(CVehicle* vehicle, int seatid);
	void EnterVehiclePassenger(CVehicle* vehicle, int seatid);
	void HandleTask(Packets::Players::SetPlayerTask& packet);
	void ApplyWeaponSnapshot(Packets::Players::SWeaponSnapshot& weaponSnapshot);
};

