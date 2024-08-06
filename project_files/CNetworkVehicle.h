#pragma once
class CNetworkVehicle
{
public:
	int m_nVehicleId = -1;
	CVehicle* m_pVehicle = nullptr;
	CNetworkVehicle(CVehicle* vehicle);
	CNetworkVehicle(int vehicleid, int modelid, CVector pos, float rotation);
	~CNetworkVehicle();
};

