// Server stand-in for plugin-sdk CVehicle.h: only what the shared packets need.
// Keep in sync with third_party/plugin-sdk/plugin_sa/game_sa/CVehicle.h
#pragma once

#include "CDamageManager.h"

enum eDoorLock : unsigned int
{
    DOORLOCK_NOT_USED,
    DOORLOCK_UNLOCKED,
    DOORLOCK_LOCKED,
    DOORLOCK_LOCKOUT_PLAYER_ONLY,
    DOORLOCK_LOCKED_PLAYER_INSIDE,
    DOORLOCK_COP_CAR,
    DOORLOCK_FORCE_SHUT_DOORS,
    DOORLOCK_SKIP_SHUT_DOORS
};

enum eVehicleType
{
    VEHICLE_AUTOMOBILE,
    VEHICLE_MTRUCK,
    VEHICLE_QUAD,
    VEHICLE_HELI,
    VEHICLE_PLANE,
    VEHICLE_BOAT,
    VEHICLE_TRAIN,
    VEHICLE_FHELI,
    VEHICLE_FPLANE,
    VEHICLE_BIKE,
    VEHICLE_BMX,
    VEHICLE_TRAILER
};

enum eVehicleCreatedBy
{
    RANDOM_VEHICLE = 1,
    MISSION_VEHICLE = 2,
    PARKED_VEHICLE = 3,
    PERMANENT_VEHICLE = 4
};
