#if !defined(_COOPSERVER_VEHICLE_VEHICLEMANAGER_H_) || !defined(_COOPSERVER_CVEHICLEMANAGER_H_)
#define _COOPSERVER_VEHICLE_VEHICLEMANAGER_H_
#define _COOPSERVER_CVEHICLEMANAGER_H_

#include "CVehicle.h"

class CVehicleManager
{
public:
	static std::vector<CVehicle*> m_pVehicles;
	static void Add(CVehicle* vehicle);
	static void Remove(CVehicle* vehicle);
	static CVehicle* GetVehicle(int vehicleid);
	static int GetFreeID();
	static void RemoveAllHostedAndNotify(CPlayer* player);
};

#endif