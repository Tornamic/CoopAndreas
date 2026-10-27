// Server stand-in for plugin-sdk CWeapon.h: only what the shared packets need.
// Keep in sync with third_party/plugin-sdk/plugin_sa/game_sa/CWeapon.h
#pragma once

#include "eWeaponType.h"
#include "CVector2D.h"
#include "ePedPieceTypes.h"

enum eWeaponState : unsigned int
{
    WEAPONSTATE_READY,
    WEAPONSTATE_FIRING,
    WEAPONSTATE_RELOADING,
    WEAPONSTATE_OUT_OF_AMMO,
    WEAPONSTATE_MELEE_MADECONTACT
};
