#if !defined(_COOPSERVER_PED_CPED_H_) || !defined(_COOPSERVER_CPED_H_)
#define _COOPSERVER_PED_CPED_H_
#define _COOPSERVER_CPED_H_ 

#include "../../include/enet/enet.h"

#include "../Vector/CVector.h"
#include "../Network/CNetwork.h"
#include "../Network/CPacket.h"
#include "../Player/CPlayer.h"

class CPed
{
	//private:
		//CPed() {} // ?
	public:
		int m_nPedId;
		short m_nModelId;
		unsigned char m_nPedType;
		unsigned char m_nCreatedBy;
		char m_szSpecialModelName[8];
		CPlayer* m_pSyncer; 
		CVector m_vecPos;

		CPed(int pedid, CPlayer* syncer, short modelId, unsigned char pedType, CVector pos, unsigned char createdBy);

		~CPed() {}
};
#endif