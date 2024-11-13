#pragma once
#include "stdafx.h"

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
	int16_t m_nMiscComponentAngle; // hydra thrusters
	float m_fPlaneGearState;
	uint8_t m_nDoorLock;

    VehicleSyncData()
        : m_vecPosition(0.0f, 0.0f, 0.0f),
        m_vecVelocity(0.0f, 0.0f, 0.0f),
        m_vecRotation(0.0f, 0.0f, 0.0f),
        m_vecRoll(0.0f, 0.0f, 0.0f),
        m_nPrimaryColor(0),
        m_nSecondaryColor(0),
        m_nHealth(1000.0f),
        m_nPaintjob(0), // possible crash
        m_fBikeLean(0.0f),
        m_fTurretAimHorizontal(0.0f),
        m_fTurretAimVertical(0.0f),
        m_nMiscComponentAngle(0),
        m_fPlaneGearState(0.0f),
        m_nDoorLock(0)
    {
    }
};