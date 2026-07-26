#pragma once

#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <unordered_map>
#include <iostream>
#include <filesystem>

#ifdef _WIN32
#define strncasecmp _strnicmp
#define strcasecmp  _stricmp
#else
#include <strings.h>
#endif

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#include "enet/enet.h"
#include "INIReader/cpp/INIReader.h"

#include "config.h"
#include "semver.h"
#include "logger.h"

#include "CNetwork.h"

#include "CNetworkPlayer.h"
#include "CNetworkVehicle.h"
#include "CNetworkPed.h"

#include "CNetworkPlayerManager.h"
#include "CNetworkVehicleManager.h"
#include "CNetworkPedManager.h"

#include "ConfigManager.h"

#include "CPacketFactory.h"

#include "network/packet.h"
#include "network/packet_types.h"
#include "network/packet_handler.h"

#include "CServerTime.h"
