#pragma once
#define MAX_SERVER_PLAYERS 4

#define _DEV


// networking ------------------------------------------------------------------------------

#include "enet/enet.h"

// windows ---------------------------------------------------------------------------------

#include <Windows.h>

// plugin-sdk ------------------------------------------------------------------------------

#include "plugin.h"
#include "Patch.h"
#include "CPlayerPed.h"
#include "CStreaming.h"
#include "CWorld.h"
#include "RenderWare.h"
#include "d3dx9.h"
#include "extensions/ScriptCommands.h"
#include "CTaskSimpleDuckToggle.h"
#include "CTaskSimplePlayerOnFoot.h"

// classes ---------------------------------------------------------------------------------

using namespace plugin;

#include "CCore.h"
#include "CNetwork.h"
#include "CPatch.h"
#include "CHook.h"
#include "CDXFont.h"
#include "Events.h"
#include "CUtil.h"
#include "CPackets.h"

// -----------------------------------------------------------------------------------------
