#pragma once
class CPed
{
public:
	int m_nPedId;
	short m_nModelId;
	unsigned char m_nPedType; 
	CVector m_vecPos;
	unsigned char m_nCreatedBy;
	CPed(int pedid, short modelId, unsigned char pedType, CVector pos, unsigned char createdBy);
};