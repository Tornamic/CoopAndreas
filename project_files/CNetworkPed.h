#pragma once
class CNetworkPed
{
private:
	CNetworkPed() {}
public:
	int m_nPedId = -1;
	CPed* m_pPed = nullptr;
	bool m_bSyncing = false;
	unsigned char m_nTempId = 255;
	ePedType m_nPedType;
	unsigned char m_nCreatedBy;
	CVector m_vecVelocity{0.0f, 0.0f, 0.0f};
	float m_fAimingRotation = 0.0f;
	float m_fCurrentRotation = 0.0f;
	int m_fLookDirection;
	eMoveState m_nMoveState = eMoveState::PEDMOVE_NONE;
	CAutoPilot m_autoPilot;
	float m_fGasPedal = 0.0f;
	float m_fBreakPedal = 0.0f;
	float m_fSteerAngle = 0.0f;

	static CNetworkPed* CreateHosted(CPed* ped);
	void WarpIntoVehicleDriver(CVehicle* vehicle);
	void WarpIntoVehiclePassenger(CVehicle* vehicle, int seatid);
	void RemoveFromVehicle(CVehicle* vehicle);
	CNetworkPed(int pedid, int modelId, ePedType pedType, CVector pos, unsigned char createdBy);
	~CNetworkPed();
};

