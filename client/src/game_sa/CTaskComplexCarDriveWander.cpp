#include "stdafx.h"
#include "CTaskComplexCarDriveWander.h"

// 0x63CB10
CTaskComplexCarDriveWander::CTaskComplexCarDriveWander(CVehicle* vehicle, uint8_t carDrivingStyle, float fSpeed)
    : CTaskComplexCarDrive(vehicle, fSpeed, (eModelID)-1, carDrivingStyle)
{
    plugin::CallMethod<0x63CB10, CTaskComplexCarDriveWander*>(this, vehicle, carDrivingStyle, fSpeed);
}