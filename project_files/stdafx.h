#pragma once
#define MAX_SERVER_PLAYERS 4
#define MAX_SERVER_VEHICLES 200
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
#include "CRadar.h"
#include "CText.h"
#include "CCamera.h"
#include "CBulletTraces.h"
#include "CDraw.h"
#include "CMenuManager.h"
#include "CSprite.h"
#include "CFont.h"
#include "CHud.h"
#include "CModelInfo.h"
#include "CTxdStore.h"
#include "CKeyGen.h"
#include "CAnimManager.h"
#include "CMessages.h"
#include "CExplosion.h"
#include "CMonsterTruck.h"
#include "CQuadBike.h"
#include "CHeli.h"
#include "CPlane.h"
#include "CBmx.h"
#include "CTrailer.h"
#include "CBoat.h"
#include "CTaskComplexEnterCarAsDriver.h"
#include "CTaskComplexLeaveCar.h"
#include "CCarCtrl.h"
// classes ---------------------------------------------------------------------------------

using namespace plugin;

#include "CCore.h"
#include "structs.h"
#include "CNetworkVehicle.h"
#include "CPackets.h"
#include "CPacketHandler.h"
#include "CNetwork.h"
#include "CNetworkPlayer.h"
#include "CNetworkPlayerManager.h"
#include "CUtil.h"
#include "CPatch.h"
#include "CHook.h"
#include "CDXFont.h"
#include "Events.h"
#include "CLocalPlayer.h"
#include "CNetworkPlayerMapPin.h"
#include "CNetworkPlayerWaypoint.h"
#include "CLaunchManager.h"
#include "CNetworkPlayerNameTag.h"
#include "CChat.h"
#include "CNetworkVehicleManager.h"

// -----------------------------------------------------------------------------------------
