#include "stdafx.h"
#include "MissionAudioHooks.h"

static int aiLoadedAudioId[4];

void __fastcall CAudioEngine__PreloadMissionAudio_Hook(CAudioEngine* This, SKIP_EDX, uint8_t slot, int audioid)
{
	plugin::CallMethod<0x507290>(This, slot, audioid);

	if (CLocalPlayer::m_bIsHost)
	{
		aiLoadedAudioId[slot] = audioid;
	}
}

void __fastcall CAudioEngine__PlayLoadedMissionAudio_Hook(CAudioEngine* This, SKIP_EDX, uint8_t slot)
{
	plugin::CallMethod<0x5072B0>(This, slot);

	if (CLocalPlayer::m_bIsHost)
	{
		CPackets::PlayMissionAudio packet{};
		packet.audioid = aiLoadedAudioId[slot];
		packet.slotid = slot;
		CNetwork::SendPacket(CPacketsID::PLAY_MISSION_AUDIO, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
	}
}

void MissionAudioHooks::InjectHooks()
{
	patch::RedirectCall(0x4851BB, CAudioEngine__PreloadMissionAudio_Hook);
	patch::RedirectCall(0x48521C, CAudioEngine__PlayLoadedMissionAudio_Hook);
}
