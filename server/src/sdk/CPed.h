// Server stand-in for plugin-sdk CPed.h: only what the shared packets need.
// Keep in sync with third_party/plugin-sdk/plugin_sa/game_sa/CPed.h
#pragma once

#include "ePedType.h"

enum PLUGIN_API eMoveState
{
    PEDMOVE_NONE = 0,
    PEDMOVE_STILL,
    PEDMOVE_TURN_L,
    PEDMOVE_TURN_R,
    PEDMOVE_WALK,
    PEDMOVE_JOG,
    PEDMOVE_RUN,
    PEDMOVE_SPRINT
};
