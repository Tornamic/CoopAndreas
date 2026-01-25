#pragma once
class CNetworkVehicle
{
private:
	CNetworkVehicle() {}
public:
	int m_nVehicleId = -1;
	CVehicle* m_pVehicle = nullptr;
	int m_nModelId = 0;
	char m_nPaintJob = -1;
	bool m_bSyncing = false;
	unsigned char m_nTempId = 255;
	unsigned char m_nCreatedBy;
	int m_nBlipHandle = -1;

	~CNetworkVehicle();
	CNetworkVehicle(int vehicleid, int modelid, CVector pos, float rotation, unsigned char color1, unsigned char color2, unsigned char createdBy);
	bool CreateVehicle(int vehicleid, int modelid, CVector pos, float rotation, unsigned char color1, unsigned char color2);
	bool HasDriver();
	
	static CNetworkVehicle* CreateHosted(CVehicle* vehicle);
};

