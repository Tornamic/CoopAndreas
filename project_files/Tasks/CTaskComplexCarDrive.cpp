#include "stdafx.h"
#include "CTaskComplexCarDrive.h"

// 0x63C9D0
CTaskComplexCarDrive::CTaskComplexCarDrive(CVehicle* vehicle) : CTaskComplex(plugin::dummy)
{
    plugin::CallMethod<0x63C9D0, CTaskComplexCarDrive*>(this, vehicle);
}

// 0x63C940
CTaskComplexCarDrive::CTaskComplexCarDrive(CVehicle* vehicle, float speed, eModelID carModelIndexToCreate, uint8_t carDrivingStyle) : CTaskComplex(plugin::dummy)
{
    plugin::CallMethod<0x63C940, CTaskComplexCarDrive*>(this, vehicle, speed, carModelIndexToCreate, carDrivingStyle);
}