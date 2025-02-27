#include "stdafx.h"
#include "CAimSync.h"

CPackets::PlayerAimSync storedAimData{};
unsigned short storedCamMode;

void ApplyPacketToGame(const CPackets::PlayerAimSync packet, int playerInfoId = 0)
{
	CCam* camera = &TheCamera.m_aCams[TheCamera.m_nActiveCam];

	camera->m_fFOV = packet.cameraFov;
	camera->m_nMode = (eCamMode)packet.cameraMode;
	TheCamera.m_PlayerWeaponMode.m_nMode = packet.cameraMode;
	camera->m_vecFront = packet.front;
	camera->m_vecSource = packet.source;
	camera->m_vecSourceBeforeLookBehind = packet.source;
	camera->m_vecUp = packet.up;

	CWorld::Players[playerInfoId].m_PlayerData.m_fFPSMoveHeading = packet.moveHeading;
}

void CAimSync::ApplyNetworkPlayerContext(CNetworkPlayer* player)
{
	storedAimData = CPacketHandler::PlayerAimSync__Collect();
	storedCamMode = TheCamera.m_PlayerWeaponMode.m_nMode;

	ApplyPacketToGame(player->m_aimSyncData, player->GetInternalId());
	if (TheCamera.m_PlayerWeaponMode.m_nMode == MODE_FOLLOWPED || TheCamera.m_PlayerWeaponMode.m_nMode == MODE_SNIPER)
	{
		TheCamera.m_PlayerWeaponMode.m_nMode = MODE_NONE;
	}
	// camera context switching patches, fix right click freeze

	// disable CCamera::ClearPlayerWeaponMode
	// ret
	patch::SetUChar(0x50AB10, 0xC3, false);

	// disable CCamera::SetNewPlayerWeaponMode
	// ret    0xC
	patch::SetRaw(0x50BFB0, "\xC2\x0C\x00", 3, false);

	// disable CCamera::Using1stPersonWeaponMode
	// mov    al,0x1
	// ret
	patch::SetRaw(0x50BFF0, "\xB0\x01\xC3", 3, false);

	// disable CPlayerPed::ClearWeaponTarget
	// ret
	patch::SetUChar(0x609C80, 0xC3, false);
}

void CAimSync::ApplyLocalContext()
{
	// return original bytes
	patch::SetUChar(0x50AB10, 0x33, false);				// CCamera::ClearPlayerWeaponMode
	patch::SetRaw(0x50BFB0, "\x66\x8B\x44", 3, false);	// CCamera::SetNewPlayerWeaponMode
	patch::SetRaw(0x50BFF0, "\x66\x8B\x81", 3, false);	// CCamera::Using1stPersonWeaponMode
	patch::SetUChar(0x609C80, 0x57, false);				// CPlayerPed::ClearWeaponTarget

	ApplyPacketToGame(storedAimData);
	TheCamera.m_PlayerWeaponMode.m_nMode = storedCamMode;
}
