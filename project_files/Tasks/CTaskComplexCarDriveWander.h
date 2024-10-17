#pragma once

#include "CTaskComplexCarDrive.h"

class PLUGIN_API CTaskComplexCarDriveWander : public CTaskComplexCarDrive {
protected:
    CTaskComplexCarDriveWander(plugin::dummy_func_t a) : CTaskComplexCarDrive(a) {}
public:
    CTaskComplexCarDriveWander(CVehicle* vehicle, uint8_t carDrivingStyle = 0, float fSpeed = 10.f);
};

VALIDATE_SIZE(CTaskComplexCarDrive, 0x24);


