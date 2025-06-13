#if !defined(_COOPSERVER_VEHICLE_VEHICLE_H_) || !defined(_COOPSERVER_CVEHICLE_H_)
#define _COOPSERVER_VEHICLE_VEHICLE_H_
#define _COOPSERVER_CVEHICLE_H_

#include <iostream>
#include <vector>
#include <algorithm>

#include "../Vector/CVector.h"
#include "../Player/CPlayerManager.h"

class CVehicle
{
	public:
		
		int m_nVehicleId;
		unsigned short m_nModelId;
		unsigned char m_nPrimaryColor;
		unsigned char m_nSecondaryColor;
		unsigned char m_damageManager_padding[23] = { 0 };
		CPlayer* m_pSyncer = nullptr;
		CVector m_vecPosition;
		CVector m_vecRotation;
		CVector m_vecVelocity;
		CPlayer* m_pPlayers[8] = { nullptr };
		std::vector<int> m_pComponents;
		uint8_t m_nCreatedBy;

		CVehicle(int vehicleid, unsigned short model, CVector pos, float rot);
		void ReassignSyncer(CPlayer* newSyncer);
		void SetOccupant(uint8_t seatid, CPlayer* player);

		~CVehicle() {}
};


#endif