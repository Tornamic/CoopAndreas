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
	if (TheCamera.m_PlayerWeaponMode.m_nMode == 4)
	{
		TheCamera.m_PlayerWeaponMode.m_nMode = (eCamMode)0;
	}
}

void CAimSync::ApplyLocalContext()
{
	ApplyPacketToGame(storedAimData);
	TheCamera.m_PlayerWeaponMode.m_nMode = storedCamMode;
}
