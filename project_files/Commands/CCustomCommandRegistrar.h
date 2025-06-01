#pragma once

#include "CCustomCommandMgr.h"
#include "Commands/CCommandAddChatMessage.h"
#include "Commands/CCommandGetNetworkPlayerPedHandleTo.h"
#include "Commands/CCommandCollectNetworkPlayersForTheMission.h"
#include "Commands/CCommandIsNetworkPlayerActorValid.h"
#include "Commands/CCommandUpdateCarBlipForNetworkPlayer.h"
#include "Commands/CCommandRemoveCarBlipForNetworkPlayer.h"
#include "Commands/CCommandShowTextLowPriorityForNetworkPlayer.h"
#include "Commands/CCommandShowTextStyledForNetworkPlayer.h"
#include "Commands/CCommandShowTextHighPriorityForNetworkPlayer.h"
#include "Commands/CCommandShowTextBoxForNetworkPlayer.h"
#include "Commands/CCommandRemoveTextForNetworkPlayer.h"
#include "Commands/CCommandUpdateActorBlipForNetworkPlayer.h"
#include "Commands/CCommandRemoveActorBlipForNetworkPlayer.h"
#include "Commands/CCommandClearAllEntityBlipsForNetworkPlayer.h"
#include "Commands/CCommandIsHost.h"
#include "Commands/CCommandLocateAllPlayersOnFoot3D.h"
#include "Commands/CCommandUpdateCheckpointForNetworkPlayer.h"
#include "Commands/CCommandRemoveCheckpointForNetworkPlayer.h"
#include "Commands/CCommandEnableSyncingThisScript.h"
#include "Commands/CCommandGetVehicleNetworkId.h"
#include "Commands/CCommandGetPedNetworkId.h"

class CCustomCommandRegistrar
{
public:
	static void Register()
	{
		CCustomCommandMgr::RegisterCommand(0x1D00, new CCommandAddChatMessage());
		CCustomCommandMgr::RegisterCommand(0x1D01, new CCommandGetNetworkPlayerPedHandleTo());
		CCustomCommandMgr::RegisterCommand(0x1D02, new CCommandCollectNetworkPlayersForTheMission());
		CCustomCommandMgr::RegisterCommand(0x1D03, new CCommandIsNetworkPlayerActorValid());
		CCustomCommandMgr::RegisterCommand(0x1D04, new CCommandUpdateCarBlipForNetworkPlayer());
		CCustomCommandMgr::RegisterCommand(0x1D05, new CCommandRemoveCarBlipForNetworkPlayer());
		CCustomCommandMgr::RegisterCommand(0x1D06, new CCommandUpdateActorBlipForNetworkPlayer());
		CCustomCommandMgr::RegisterCommand(0x1D07, new CCommandRemoveActorBlipForNetworkPlayer());
		CCustomCommandMgr::RegisterCommand(0x1D08, new CCommandShowTextLowPriorityForNetworkPlayer());
		CCustomCommandMgr::RegisterCommand(0x1D09, new CCommandShowTextStyledForNetworkPlayer());
		CCustomCommandMgr::RegisterCommand(0x1D0A, new CCommandShowTextHighPriorityForNetworkPlayer());
		CCustomCommandMgr::RegisterCommand(0x1D0B, new CCommandShowTextBoxForNetworkPlayer());
		CCustomCommandMgr::RegisterCommand(0x1D0C, new CCommandRemoveTextForNetworkPlayer());
		CCustomCommandMgr::RegisterCommand(0x1D0D, new CCommandClearAllEntityBlipsForNetworkPlayer());
		CCustomCommandMgr::RegisterCommand(0x1D0E, new CCommandIsHost());
		CCustomCommandMgr::RegisterCommand(0x1D0F, new CCommandLocateAllPlayersOnFoot3D());
		CCustomCommandMgr::RegisterCommand(0x1D10, new CCommandUpdateCheckpointForNetworkPlayer());
		CCustomCommandMgr::RegisterCommand(0x1D11, new CCommandRemoveCheckpointForNetworkPlayer());
		CCustomCommandMgr::RegisterCommand(0x1D12, new CCommandEnableSyncingThisScript());
		CCustomCommandMgr::RegisterCommand(0x1D13, new CCommandGetVehicleNetworkId());
		CCustomCommandMgr::RegisterCommand(0x1D14, new CCommandGetPedNetworkId());
	}
};