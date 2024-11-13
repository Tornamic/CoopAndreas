#pragma once

#include "CVector.h"

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


};

