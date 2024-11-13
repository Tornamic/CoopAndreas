#pragma once

#include "CVector.h"

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

    PedSyncData()
        : m_vecPosition(0.0f, 0.0f, 0.0f),
        m_vecVelocity(0.0f, 0.0f, 0.0f),
        m_nHealth(100),
        m_nArmour(0),
        m_nCurrentWeapon(0),
        m_nAmmoInClip(0),
        m_fAimingRotation(0.0f),
        m_fCurrentRotation(0.0f),
        m_fLookDirection(0.0f),
        m_nMoveState(0)
    {
    }
};