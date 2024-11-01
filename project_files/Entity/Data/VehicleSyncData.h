#pragma once

#include "CVector.h"

struct VehicleSyncData
{
public:
	CVector m_vecPosition;
	CVector	m_vecVelocity;
	CVector	m_vecRotation;
	CVector m_vecRoll;
	uint8_t m_nPrimaryColor;
	uint8_t m_nSecondaryColor;
	float m_nHealth;
	int8_t m_nPaintjob;
	float m_fBikeLean;
	float m_fTurretAimHorizontal;
	float m_fTurretAimVertical;
	uint16_t m_nMiscComponentAngle; // hydra thrusters
	float m_fPlaneGearState;
	bool m_bLocked;
};