#pragma once
class CUtil
{
public:
	static bool CompareControllerStates(const CControllerState& state1, const CControllerState& state2);
	static void CopyControllerState(CControllerState& destination, const CControllerState& source);
	static bool IsDucked(CPed* ped);
	static bool isDifferenceGreaterThanPercent(float value1, float value2, int percent);
	static bool IsPositionUpdateNeeded(CVector pos, CVector update, int percent = 5);
	static int GetWeaponModelById(unsigned char id);
	static bool IsMeleeWeapon(unsigned char id);
	static void GiveWeaponByPacket(CNetworkPlayer* player, unsigned char weapon, unsigned short ammo, bool select = true);
	static void GiveWeaponByPacket(CNetworkPed* ped, unsigned char weapon, unsigned short ammo, bool select = true);
	static bool IsVehicleHasTurret(CVehicle* vehicle);
	static eVehicleType GetVehicleType(CVehicle* vehicle);
	static CNetworkPed* GetNetworkPedByTask(CTask* targetTask);
	static bool IsPedHasJetpack(CPed* ped);
	static void SetPlayerJetpack(CNetworkPlayer* ped, bool set);
	static bool IsValidEntityPtr(CEntity* ptr);
	static inline float SCREEN_STRETCH_X(float a) { return a * RsGlobal.maximumWidth / (float)640; }
	static inline float SCREEN_STRETCH_Y(float a) { return a * RsGlobal.maximumHeight / (float)448; }
};

