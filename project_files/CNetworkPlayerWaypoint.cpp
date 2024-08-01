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
	}
}