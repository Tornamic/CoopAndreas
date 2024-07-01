#pragma once

#define _DEV

// networking ------------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "../enet/enet.h"

// plugin-sdk ------------------------------------------------------------------------------


#include "plugin.h"
#include "Patch.h"

// classes ---------------------------------------------------------------------------------

using namespace plugin;

#include "CCore.h"
#include "CNetwork.h"
#include "CPatch.h"
#include "CHook.h"

// -----------------------------------------------------------------------------------------

