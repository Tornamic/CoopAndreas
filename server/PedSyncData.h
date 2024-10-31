#pragma once
#include "stdafx.h"

struct PedSyncData
{
public:
	CVector m_vecPosition;
	CVector m_vecVelocity;
	uint8_t m_nHealth;
	uint8_t m_nArmour;
	uint8_t m_nCurrentWeapon;
	uint16_t m_nAmmoInClip;
	float m_fAimingRotation;
	float m_fCurrentRotation;
	float m_fLookDirection;
	uint8_t m_nMoveState;
};