#pragma once

#include "CTaskComplex.h"
#include <eModelID.h>
#include "eCarMission.h"

class CVehicle;

class PLUGIN_API CTaskComplexCarDrive : public CTaskComplex {
protected:
    CTaskComplexCarDrive(plugin::dummy_func_t a) : CTaskComplex(a) {}
public:
    CTaskComplexCarDrive(CVehicle* vehicle);
    CTaskComplexCarDrive(CVehicle* vehicle, float speed, eModelID carModelIndexToCreate = (eModelID)-1, uint8_t carDrivingStyle = 0);

    CVehicle* m_Veh{};
    float     m_CruiseSpeed{};
    eModelID  m_DesiredCarModel{ (eModelID)-1};
    uint8_t   m_CarDrivingStyle{ 0 }; // DRIVING_STYLE_STOP_FOR_CARS
    bool      m_bAsDriver{};
    char      m_OriginalDrivingStyle{};
    char      m_OriginalMission{};
    uint8_t   m_OriginalSpeed{};
    bool      m_bIsCarSetUp{};
    char      padding[6];
};

VALIDATE_SIZE(CTaskComplexCarDrive, 36);

