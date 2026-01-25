#pragma once

#ifndef _CPED_H_
	#define _CPED_H_
#include "enet/enet.h"

#include "CVector.h"
#include "CNetwork.h"
#include "CPacket.h"
#include "CPlayerManager.h"

class CPed
{
private:
	CPed() {}
public:
	int m_nPedId;
	CPlayer* m_pSyncer;
	short m_nModelId;
	unsigned char m_nPedType; 
	CVector m_vecPos;
	unsigned char m_nCreatedBy;
	char m_szSpecialModelName[8];
	CPed(int pedid, CPlayer* syncer, short modelId, unsigned char pedType, CVector pos, unsigned char createdBy);
};

#endif