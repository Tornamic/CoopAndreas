#pragma once

#define MAX_SERVER_PLAYERS 4

#define _DEV

// networking ------------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "../enet/enet.h"

// plugin-sdk ------------------------------------------------------------------------------

#include "plugin.h"
#include "Patch.h"
#include "CPlayerPed.h"
#include "CStreaming.h"
#include "CWorld.h"
#include "RenderWare.h"
#include "d3dx9.h"

// classes ---------------------------------------------------------------------------------

using namespace plugin;

#include "CCore.h"
#include "CNetwork.h"
#include "CPatch.h"
#include "CHook.h"
#include "CNetworkPlayer.h"
#include "CDXFont.h"

// -----------------------------------------------------------------------------------------

