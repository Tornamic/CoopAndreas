#pragma once
class CNetworkPed
{
public:
	int m_nPedId = -1;
	CPed* m_pPed = nullptr;
	ePedType m_nPedType;
	unsigned char m_nCreatedBy;
	CVector m_vecVelocity{0.0f, 0.0f, 0.0f};
	float m_fAimingRotation = 0.0f;
	float m_fCurrentRotation = 0.0f;
	float m_fLookDirection = 0.0f;
	eMoveState m_nMoveState = eMoveState::PEDMOVE_NONE;
	CAutoPilot m_autoPilot;
	float m_fGasPedal = 0.0f;
	float m_fBreakPedal = 0.0f;
	float m_fSteerAngle = 0.0f;

	CNetworkPed(CPed* ped);
	CNetworkPed(int pedid, int modelId, ePedType pedType, CVector pos, unsigned char createdBy);
	~CNetworkPed();
};

