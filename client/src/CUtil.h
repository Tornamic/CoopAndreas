#pragma once

class CUtil
{
public:
	static constexpr float DEFAULT_ASPECT_RATIO = 4.0f / 3.0f;
	static constexpr float SCREEN_BASE_WIDTH = 640.0f;
	static constexpr float SCREEN_BASE_HEIGHT = 448.0f;

	static bool CompareControllerStates(const CControllerState& state1, const CControllerState& state2);
	static void CopyControllerState(CControllerState& destination, const CControllerState& source);
	static bool IsDucked(CPed* ped);
	static bool isDifferenceGreaterThanPercent(float value1, float value2, int percent);
	static bool IsPositionUpdateNeeded(CVector pos, CVector update, int percent = 5);
	static int GetWeaponModelById(unsigned char id);
	static bool IsMeleeWeapon(unsigned char id);
	static void GiveWeaponByPacket(CNetworkPlayer* player, unsigned char weapon, unsigned short ammo, bool select = true);
	static void GiveWeaponByPacket(CNetworkPed* ped, unsigned char weapon, unsigned short ammo, bool select = true);
	static eVehicleType GetVehicleType(CVehicle* vehicle);
	static CNetworkPed* GetNetworkPedByTask(CTask* targetTask);
	static bool IsPedHasJetpack(CPed* ped);
	static void SetPlayerJetpack(CNetworkPlayer* ped, bool set);
	static std::string GetWeaponName(eWeaponType type);
	static float HUD_X(float a) { return a * RsGlobal.maximumWidth / SCREEN_BASE_WIDTH; }
	static float HUD_Y(float a) { return a * RsGlobal.maximumHeight / SCREEN_BASE_HEIGHT; }
	static float SCREEN_SCALE_AR(float a) { return a * DEFAULT_ASPECT_RATIO / CDraw::ms_fAspectRatio; };
	static float SCREEN_SCALE_X(float a) { return SCREEN_SCALE_AR(HUD_X(a)); }
	static float SCREEN_SCALE_Y(float a) { return HUD_Y(a); }

};

