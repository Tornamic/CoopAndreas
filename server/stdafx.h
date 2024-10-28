#pragma once
#define MAX_SERVER_PLAYERS 4
#define STREAM_DISTANCE (100.0f) // todo
#define STREAM_OUT_STEP (10.0f)

// -----------------------------------------------------------------------------------------

#include "enet/enet.h"

// -----------------------------------------------------------------------------------------

#include "iostream"
#include "stdbool.h"
#include "string"
#include "vector"



// classes ---------------------------------------------------------------------------------

#include "CVector.h"
#include "CControllerState.h"

#include "CNetwork.h"
#include "CPlayer.h"
#include "CVehicle.h"
#include "CPed.h"
#include "CPlayerManager.h"
#include "CVehicleManager.h"
#include "CPedManager.h"

#include "CPackets.h"