// Server stand-in for plugin-sdk CExplosion.h: only what the shared packets need.
// Keep in sync with third_party/plugin-sdk/plugin_sa/game_sa/CExplosion.h
#pragma once

#include "CVector.h"

enum eExplosionType
{
    EXPLOSION_GRENADE,
    EXPLOSION_MOLOTOV,
    EXPLOSION_ROCKET,
    EXPLOSION_WEAK_ROCKET,
    EXPLOSION_CAR,
    EXPLOSION_QUICK_CAR,
    EXPLOSION_BOAT,
    EXPLOSION_AIRCRAFT,
    EXPLOSION_MINE,
    EXPLOSION_OBJECT,
    EXPLOSION_TANK_FIRE,
    EXPLOSION_SMALL,
    EXPLOSION_RC_VEHICLE
};
