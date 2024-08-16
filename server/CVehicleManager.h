#pragma once
class CVehicleManager
{
public:
	static std::vector<CVehicle*> m_pVehicles;
	static void Add(CVehicle* vehicle);
	static void Remove(CVehicle* vehicle);
	static CVehicle* GetVehicle(int vehicleid);
};

