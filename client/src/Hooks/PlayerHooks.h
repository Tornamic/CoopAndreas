#pragma once
class PlayerHooks
{
public:
	static void InjectHooks();

	// a bullet of the local player as the game's own shot goes: synced to the others (the stand's shoot-wave)
	static void DoLocalPlayerBulletImpact(CWeapon* weapon, CEntity* victim, CVector* startPoint, CVector* endPoint,
		CColPoint* colPoint);
};

