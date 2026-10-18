#pragma once

class CNetworkPlayer
{
public:
	CPlayerPed* m_pPed = nullptr;
	playerid_t m_playerId;

	// reusable [0; MAX_SERVER_PLAYERS) index for the game player arrays, unlike `m_playerId`
	int m_nSlot = -1;

	Packets::Players::OnFootUpdate m_onFootSnapshotInterpolated{};
	
	uint32_t m_startedInterpolatingCameraAt = 0;
	Packets::Players::PlayerCameraSync m_cameraSnapshotOld{};
	Packets::Players::PlayerCameraSync m_cameraSnapshot{};
	
	signed short m_oShockButtonL;
	signed short m_lShockButtonL;

	Packets::Players::PlayerPlaceWaypoint m_waypointState{};

	char m_Name[Config::MAX_NICKNAME_LENGTH + 1] = { 0 };
	uint16_t m_nRTT = 0;

	CControllerState m_oldControllerState{};
	CControllerState m_newControllerState{};

	CNetworkPlayerStats m_stats{};
	CPedClothesDesc m_pPedClothesDesc{};
	bool m_bHasBeenConnectedBeforeMe = false;

	bool m_bExtrapolating = false;

	bool m_bRequestedDuckTask = false;

	bool m_bIsHost = false;

	~CNetworkPlayer();
	CNetworkPlayer(playerid_t id, CVector position);

	void CreatePed(CVector position);
	void DestroyPed();
	void Respawn();
	int GetInternalId();
	std::string GetName();
	char GetWeaponSkill(eWeaponType weaponType);
	float GetMaxHealth();
	void WarpIntoVehicleDriver(CVehicle* vehicle);
	void RemoveFromVehicle(CVehicle* vehicle);
	void WarpIntoVehiclePassenger(CVehicle* vehicle, int seatid);
	void EnterVehiclePassenger(CVehicle* vehicle, int seatid);
	void HandleTask(Packets::Players::SetPlayerTask& packet);
	void ApplyWeaponSnapshot(Packets::Players::SWeaponSnapshot& weaponSnapshot);
};

