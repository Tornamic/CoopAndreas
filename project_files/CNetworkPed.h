#pragma once
class CNetworkPed
{
public:
	int m_nPedId = -1;
	CPed* m_pPed = nullptr;
	ePedType m_nPedType;
	CNetworkPed(CPed* ped);
	CNetworkPed(int pedid, int modelId, ePedType pedType, CVector pos, unsigned char createdBy);
	~CNetworkPed();
};

