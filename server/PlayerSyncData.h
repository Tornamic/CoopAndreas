#pragma once
#include "stdafx.h"

struct PlayerSyncData
{
public:
	CVector m_vecPosition;
	CVector	m_vecVelocity;
	float m_fRotation;
	uint8_t m_nHealth;
	uint8_t m_nArmour;
	uint8_t m_nCurrentWeapon;
	uint16_t m_nAmmoInClip;
	bool m_bDucking;
	float m_fAimX;
	float m_fAimY;
	bool m_bHasJetpack;

    PlayerSyncData()
        : m_vecPosition(0.0f, 0.0f, 0.0f),
        m_vecVelocity(0.0f, 0.0f, 0.0f),  
        m_fRotation(0.0f),
        m_nHealth(100),
        m_nArmour(0),
        m_nCurrentWeapon(0),
        m_nAmmoInClip(0),
        m_bDucking(false),
        m_fAimX(0.0f),
        m_fAimY(0.0f),
        m_bHasJetpack(false)
    {
    }
};

