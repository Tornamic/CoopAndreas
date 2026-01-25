#pragma once

#ifndef _CVEHICLE_H_
	#define _CVEHICLE_H_
#include <iostream>
#include <vector>
#include <algorithm>

#include "CVector.h"
#include "CPlayerManager.h"

class CVehicle
{
	public:
		CVehicle(int vehicleid, unsigned short model, CVector pos, float rot);
		
		int m_nVehicleId;
		CPlayer* m_pSyncer = nullptr;
		unsigned short m_nModelId;
		CVector m_vecPosition;
		CVector m_vecRotation;
		CVector m_vecVelocity;
		CPlayer* m_pPlayers[8] = { nullptr };
		unsigned char m_nPrimaryColor;
		unsigned char m_nSecondaryColor;
		unsigned char m_damageManager_padding[23] = { 0 };
		std::vector<int> m_pComponents;
		uint8_t m_nCreatedBy;
		bool m_bUsedByPed = false;

		void ReassignSyncer(CPlayer* newSyncer);
		void SetOccupant(uint8_t seatid, CPlayer* player);

		~CVehicle() {}
};


#endif