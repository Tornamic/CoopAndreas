#include "network/packets/scripts.h"
#include "network/packet_types.h"
#include "stdafx.h"
#include <CNetworkCheckpoint.h>
#include <CNetworkEntityBlip.h>
#include <CEntryExitMarkerSync.h>
#include <COpCodeSync.h>
#include <CTaskSequenceSync.h>

PACKET_HANDLER(ePacketType::ON_MISSION_FLAG_SYNC, Packets::Scripts::OnMissionFlagSync* pOnMissionFlagSync)
{
	if (CLocalPlayer::m_bIsHost)
		return;

	if (CTheScripts::OnAMissionFlag)
	{
		CTheScripts::ScriptSpace[CTheScripts::OnAMissionFlag] = pOnMissionFlagSync->bOnMission;
		if (pOnMissionFlagSync->bOnMission == false && (bool)CTheScripts::ScriptSpace[CTheScripts::OnAMissionFlag])
		{
			// cleanup
			CNetworkCheckpoint::Remove();
			CNetworkEntityBlip::ClearEntityBlips();
			TheCamera.SetWideScreenOff();
			CPad::GetPad(0)->SetDrunkInputDelay(0);
			CPad::GetPad(0)->DisablePlayerControls = 0;
			CPad::GetPad(0)->bApplyBrakes = 0;
			CPad::GetPad(0)->bDisablePlayerEnterCar = 0;
			CPad::GetPad(0)->bDisablePlayerDuck = 0;
			CPad::GetPad(0)->bDisablePlayerFireWeapon = 0;
			CPad::GetPad(0)->bDisablePlayerFireWeaponWithL1 = 0;
			CPad::GetPad(0)->bDisablePlayerCycleWeapon = 0;
			CPad::GetPad(0)->bDisablePlayerJump = 0;
			CPad::GetPad(0)->bDisablePlayerDisplayVitalStats = 0;
			CDraw::FadeValue = 0;
		}
	}
}

PACKET_HANDLER(ePacketType::ENEX_SYNC, Packets::Scripts::EnExSync* pEnExSync)
{
	if (CLocalPlayer::m_bIsHost)
		return;

	CEntryExitMarkerSync::Receive(*pEnExSync);
}

PACKET_HANDLER(ePacketType::ADD_MESSAGE_GXT, Packets::Scripts::AddMessageGXT* pAddMessageGXT)
{
	if (CLocalPlayer::m_bIsHost)
		return;

	char gxt[8];
	strncpy_s(gxt, pAddMessageGXT->gxt, 8);
	gxt[7] = '\0';

	if (pAddMessageGXT->type == Packets::Scripts::AddMessageGXT::eGXTMsgType::sync_COMMAND_PRINT)
	{
		Command<Commands::PRINT>(gxt, pAddMessageGXT->time, pAddMessageGXT->flag);
	}
	else if (pAddMessageGXT->type == Packets::Scripts::AddMessageGXT::eGXTMsgType::sync_COMMAND_PRINT_BIG)
	{
		Command<Commands::PRINT_BIG>(gxt, pAddMessageGXT->time, pAddMessageGXT->flag);
	}
	else if (pAddMessageGXT->type == Packets::Scripts::AddMessageGXT::eGXTMsgType::sync_COMMAND_PRINT_NOW)
	{
		Command<Commands::PRINT_NOW>(gxt, pAddMessageGXT->time, pAddMessageGXT->flag);
	}
	else if (pAddMessageGXT->type == Packets::Scripts::AddMessageGXT::eGXTMsgType::sync_COMMAND_PRINT_HELP)
	{
		Command<Commands::PRINT_HELP>(gxt);
	}
	//CChat::AddMessage("%s %d %d %d", gxt, pAddMessageGXT->type, pAddMessageGXT->time, pAddMessageGXT->flag);
}

PACKET_HANDLER(ePacketType::REMOVE_MESSAGE_GXT, Packets::Scripts::RemoveMessageGXT* pRemoveMessageGXT)
{
	if (CLocalPlayer::m_bIsHost)
		return;

	char gxt[8];
	strncpy_s(gxt, pRemoveMessageGXT->gxt, 8);
	gxt[7] = '\0';

	Command<Commands::CLEAR_THIS_PRINT>(gxt);
	Command<Commands::CLEAR_THIS_BIG_PRINT>(gxt);
}

#if 0 // controlled with SCM
PACKET_HANDLER(ePacketType::START_CUTSCENE, Packets::Scripts::StartCutscene* pStartCutscene)
{
	if (CCutsceneMgr::ms_cutsceneLoadStatus == 2)
	{
		CCutsceneMgr::FinishCutscene();
		CCutsceneMgr::DeleteCutsceneData();
	}
	if (CCutsceneMgr::ms_cutsceneLoadStatus != 2)
	{
		CCutsceneMgr::LoadCutsceneData(pStartCutscene->name);
	}
	CGame::currArea = pStartCutscene->currArea;
	CCutsceneMgr::StartCutscene();
}

PACKET_HANDLER(ePacketType::SKIP_CUTSCENE, Packets::Scripts::SkipCutscene* pSkipCutscene)
{
	CHud::m_BigMessage[1][0] = 0;
	CCutsceneMgr::ms_wasCutsceneSkipped = 1;
	CCutsceneMgr::FinishCutscene();
}
#endif

PACKET_HANDLER(ePacketType::PLAY_MISSION_AUDIO, Packets::Scripts::PlayMissionAudio* pPlayMissionAudio)
{	
	if (CLocalPlayer::m_bIsHost)
		return;

	plugin::CallMethod<0x507290>(&AudioEngine, pPlayMissionAudio->slotid, pPlayMissionAudio->audioid); // CAudioEngine__PreloadMissionAudio
	COpCodeSync::ms_abLoadingMissionAudio[pPlayMissionAudio->slotid] = true;
}

PACKET_HANDLER(ePacketType::TELEPORT_PLAYER_SCRIPTED, Packets::Scripts::TeleportPlayerScripted* pTeleportPlayerScripted)
{
	auto pPlayerPed = FindPlayerPed(0);
	pPlayerPed->Teleport(pTeleportPlayerScripted->pos, false);
	pPlayerPed->m_fCurrentRotation = pTeleportPlayerScripted->heading.m_angle;
	pPlayerPed->m_fAimingRotation = pTeleportPlayerScripted->heading.m_angle;
	pPlayerPed->SetHeading(pTeleportPlayerScripted->heading.m_angle);
	pPlayerPed->UpdateRwMatrix();
}

PACKET_HANDLER(ePacketType::OPCODE_SYNC, Packets::Scripts::OpCodeSync* pOpCodeSync)
{
	/*if (CLocalPlayer::m_bIsHost)
		return;*/

	COpCodeSync::HandlePacket(pOpCodeSync->buffer, pOpCodeSync->size);
}


PACKET_HANDLER(ePacketType::PERFORM_TASK_SEQUENCE, Packets::Scripts::PerformTaskSequence* pPerformTaskSequence)
{
	CTaskSequenceSync::HandlePacket(pPerformTaskSequence->buffer, pPerformTaskSequence->size);
}
