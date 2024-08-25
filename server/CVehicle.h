#pragma once
class CVehicle
{
public:
	int m_nVehicleId;
	unsigned short m_nModelId;
	CVector m_vecPosition;
	CVector m_vecRotation;
	CVector m_vecVelocity;
	CPlayer* m_pPlayers[8] = { nullptr };
	unsigned char m_nPrimaryColor;
	unsigned char m_nSecondaryColor;
	unsigned char m_damageManager_padding[23] = { 0 };

	CVehicle(int vehicleid, unsigned short model, CVector pos, float rot);
};

