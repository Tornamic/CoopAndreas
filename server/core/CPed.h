#pragma once

#ifndef _CPED_H_
	#define _CPED_H_
#include "../thirdparty-libraries/enet/enet.h"

#include "CVector.h"
#include "CNetwork.h"
#include "CPacket.h"

class CPed
{
	public:
		CPed();
		
		int m_nPedId;
		short m_nModelId;
		unsigned char m_nPedType; 
		CVector m_vecPos;
		unsigned char m_nCreatedBy;
		CPed(int pedid, short modelId, unsigned char pedType, CVector pos, unsigned char createdBy);
		
		~CPed();


};

#endif