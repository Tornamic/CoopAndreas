#define MAX_SERVER_PLAYERS 4

// -----------------------------------------------------------------------------------------

#include "enet/enet.h"

// -----------------------------------------------------------------------------------------

#include "iostream"
#include "stdbool.h"
#include "string"
#include "vector"



// classes ---------------------------------------------------------------------------------

#include "CNetwork.h"
#include "CPlayer.h"
#include "CPlayerManager.h"

// structures -------------------------------------------------------------------

#include "CVector.h"
#include "CControllerState.h"
#include "CPackets.h"