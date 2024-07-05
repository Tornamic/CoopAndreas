#define MAX_SERVER_PLAYERS 4

// -----------------------------------------------------------------------------------------

#include "enet/enet.h"

// -----------------------------------------------------------------------------------------

#include "iostream"
#include "stdbool.h"
#include "string"
#include "vector"

// shared ----------------------------------------------------------------------------------

#include "../shared/Packet.h"

// classes ---------------------------------------------------------------------------------

#include "CNetwork.h"
#include "CPlayer.h"
#include "CPlayerManager.h"