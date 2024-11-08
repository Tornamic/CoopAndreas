#include "stdafx.h"
#include "CDXFont.h"
#include "CUtil.h"
#include "CNetworkPlayerWaypoint.h"
#include "Entity/Manager/Types/CNetworkPlayerManager.h"

void CNetworkPlayerWaypoint::Process()
{
	for (auto player : CNetworkPlayerManager::Instance().GetEntities())
	{
		if (!player->m_bWaypointPlaced)
			continue;

		CVector2D radar;
		CRadar::TransformRealWorldPointToRadarSpace(radar, player->m_vecWaypointPos);
		
		CRadar::LimitRadarPoint(radar);

		CVector2D screen;
		CRadar::TransformRadarPointToScreenSpace(screen, radar);

		CRadar::DrawRadarSprite(eRadarSprite::RADAR_SPRITE_WAYPOINT, screen.x, screen.y, 255);

		if (FrontEndMenuManager.m_bDrawRadarOrMap) 
		{
			screen.x = CUtil::SCREEN_STRETCH_X(screen.x) - CUtil::SCREEN_STRETCH_X(8.0f) / 2.f * 6; // 8.0f - size of the sprite, / 2 - half of the size sprite, * 6 - half of the font size
			screen.y = CUtil::SCREEN_STRETCH_Y(screen.y) - CUtil::SCREEN_STRETCH_X(8.0f) / 2.f * 5; // 5 - font size / 2 - 1

			CRadar::LimitToMap(&screen.x, &screen.y);
			CDXFont::Draw((int)screen.x, (int)screen.y, player->GetName(), D3DCOLOR_RGBA(181, 24, 24, 255));
		}
	}
}