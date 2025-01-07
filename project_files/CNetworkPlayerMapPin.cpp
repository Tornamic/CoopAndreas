#include "stdafx.h"

CVector2D GetPlayerMarkerPosition()
{
	CVector2D vec = FindPlayerCoors(-1) - CRadar::vec2DRadarOrigin;
	CVector2D playerDirection = 
	{ 
		vec.x / CRadar::m_radarRange, 
		vec.y / CRadar::m_radarRange 
	};

	CVector2D rotatedPos = {
		CRadar::cachedSin * playerDirection.y + CRadar::cachedCos * playerDirection.x,
		CRadar::cachedCos * playerDirection.y - CRadar::cachedSin * playerDirection.x
	};
	CRadar::LimitRadarPoint(rotatedPos);

	CVector2D ret{};
	
	CRadar::TransformRadarPointToScreenSpace(ret, rotatedPos);

	return ret;
};

float CalculateMarkerAngle(CNetworkPlayer* player)
{
	float baseAngle = player->m_lOnFoot->rotation;

	if (player->m_pPed->m_pVehicle != nullptr)
	{
		baseAngle = player->m_pPed->m_pVehicle->GetHeading();
	}

	if (!FrontEndMenuManager.m_bDrawRadarOrMap)
	{
		return baseAngle - CRadar::m_fRadarOrientation - (float)M_PI;
	}
	else
	{
		return baseAngle - CRadar::m_fRadarOrientation + (float)M_PI;
	}
}

void CNetworkPlayerMapPin::Process()
{
	for (auto player : CNetworkPlayerManager::m_pPlayers)
	{
		CWorld::PlayerInFocus = player->GetInternalId();

		if (CWorld::PlayerInFocus == -1 || player->m_lOnFoot == nullptr)
			continue;

		CVector2D pos = GetPlayerMarkerPosition();
		float angle = CalculateMarkerAngle(player);

		CRadar::DrawRotatingRadarSprite(
			&CRadar::RadarBlipSprites[RADAR_SPRITE_CENTRE],
			pos.x,
			pos.y,
			angle,
			5 * RsGlobal.maximumWidth / 640,
			5 * RsGlobal.maximumHeight / 360,
			player->m_pPed->IsHidden() ? CRGBA{ 50, 50, 50, 255 } : CRGBA{ 255, 255, 255, 255 }
		);
	}

	CWorld::PlayerInFocus = 0;
}