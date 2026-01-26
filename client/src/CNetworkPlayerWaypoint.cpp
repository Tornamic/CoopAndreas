#include "stdafx.h"

void CNetworkPlayerWaypoint::Process()
{
	for (auto player : CNetworkPlayerManager::m_pPlayers)
	{
		if (!player->m_bWaypointPlaced || player->m_vecWaypointPos == nullptr)
			continue;

		CVector2D radar;
		CRadar::TransformRealWorldPointToRadarSpace(radar, *player->m_vecWaypointPos);
		
		CRadar::LimitRadarPoint(radar);

		CVector2D screen;
		CRadar::TransformRadarPointToScreenSpace(screen, radar);

		CRadar::DrawRadarSprite(eRadarSprite::RADAR_SPRITE_WAYPOINT, screen.x, screen.y, 255);

		if (FrontEndMenuManager.m_bDrawRadarOrMap) 
		{
			const char* name = player->GetName().c_str();
			D3DCOLOR shadowColor = D3DCOLOR_RGBA(0, 0, 0, 255);

			// 8.0f - size of the sprite, / 2 - half of the size sprite
			screen.x = CUtil::SCREEN_STRETCH_X(screen.x);
			screen.y = CUtil::SCREEN_STRETCH_Y(screen.y) - CUtil::SCREEN_STRETCH_X(8.0f) / 2.f * offsetY;

			CRadar::LimitToMap(&screen.x, &screen.y);
			RECT rect{};
			rect.left = (LONG)screen.x;
			rect.top = (LONG)screen.y;
			rect.right = RsGlobal.maximumWidth;
			rect.bottom = RsGlobal.maximumHeight;

			CDXFont::m_pD3DXFont->DrawTextA(nullptr, name, -1, &rect, DT_CALCRECT, D3DCOLOR_RGBA(0, 0, 0, 0));
			int diff = (rect.right - rect.left) / 2;
			rect.left -= diff;
			rect.right += diff;

			RECT rightRect = rect;
			rightRect.left += CDXFont::m_iShadowSize;
			CDXFont::m_pD3DXFont->DrawTextA(nullptr, name, -1, &rightRect, 0, shadowColor);

			RECT leftRect = rect;
			leftRect.left -= CDXFont::m_iShadowSize;
			CDXFont::m_pD3DXFont->DrawTextA(nullptr, name, -1, &leftRect, 0, shadowColor);

			RECT bottomRect = rect;
			bottomRect.top += CDXFont::m_iShadowSize;
			CDXFont::m_pD3DXFont->DrawTextA(nullptr, name, -1, &bottomRect, 0, shadowColor);

			RECT topRect = rect;
			CDXFont::m_pD3DXFont->DrawTextA(nullptr, name, -1, &topRect, 0, shadowColor);
			topRect.top -= CDXFont::m_iShadowSize;

			CDXFont::m_pD3DXFont->DrawTextA(nullptr, name, -1, &rect, 0, D3DCOLOR_RGBA(181, 24, 24, 255));
		}
	}
}