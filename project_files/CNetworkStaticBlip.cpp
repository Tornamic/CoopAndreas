#include "stdafx.h"
#include "CNetworkStaticBlip.h"
#include "CEntryExit.h"
#include <CEntryExitManager.h>

void CNetworkStaticBlip::Create(CPackets::CreateStaticBlip& packet)
{
	assert(IsAllowedSyncingRadarSprite(static_cast<eRadarSprite>(packet.sprite)));

	/*if (!IsAllowedSyncingRadarSprite(sprite))
		return;*/

	//CChat::AddMessage("CNetworkMissionMarker::Create({%.2f %.2f %.2f}, %d, %d)", position.x, position.y, position.z, sprite, display);

	if (ms_bMassUpdateJustReceived)
	{
		for (int i = 0; i < MAX_RADAR_TRACES; i++)
		{
			auto& trace = CRadar::ms_RadarTrace[i];

			if ((trace.m_nBlipType == BLIP_CONTACTPOINT 
				|| trace.m_nBlipType == BLIP_COORD)
				&& IsAllowedSyncingRadarSprite(static_cast<eRadarSprite>(trace.m_nRadarSprite))
				)
			{
				CRadar::ClearActualBlip(i);
			}
		}
		ms_bMassUpdateJustReceived = false;
	}

	int blip = CRadar::SetCoordBlip(static_cast<eBlipType>(packet.type ? eBlipType::BLIP_COORD : eBlipType::BLIP_CONTACTPOINT), packet.position, 0, static_cast<eBlipDisplay>(packet.display), nullptr);
	CRadar::SetBlipSprite(blip, packet.sprite);
	CRadar::ChangeBlipDisplay(blip, static_cast<eBlipDisplay>(packet.display));

	if(const auto index = CRadar::GetActualBlipArrayIndex(blip); index != -1)
	{
		CRadar::ms_RadarTrace[index].m_bShortRange = packet.shortRange;
		CRadar::ms_RadarTrace[index].m_bFriendly = packet.friendly;
		CRadar::ms_RadarTrace[index].m_nCoordBlipAppearance = packet.coordBlipAppearance;
		CRadar::ms_RadarTrace[index].m_nBlipSize = packet.size;
	}
}

void CNetworkStaticBlip::Send()
{
	CMassPacketBuilder builder{};
	CPackets::CreateStaticBlip packet{};
	for (int i = 0; i < MAX_RADAR_TRACES; i++)
	{
		auto& trace = CRadar::ms_RadarTrace[i];

		if ((trace.m_nBlipType != eBlipType::BLIP_CONTACTPOINT
			&& trace.m_nBlipType != eBlipType::BLIP_COORD)
			|| !IsAllowedSyncingRadarSprite(static_cast<eRadarSprite>(trace.m_nRadarSprite))
			)
		{
			continue;
		}
		
		if (trace.m_pEntryExit)
		{
			auto& rect = trace.m_pEntryExit->m_recEntrance;
			packet.position = CVector((rect.right + rect.left) * 0.5f, (rect.bottom + rect.top) * 0.5f, trace.m_pEntryExit->m_fEntranceZ);
		}
		else
		{
			packet.position = trace.m_vecPos;
		}

		packet.display = trace.m_nBlipDisplay;
		packet.sprite = trace.m_nRadarSprite;
		packet.type = trace.m_nBlipType == eBlipType::BLIP_COORD;
		packet.trackingBlip = trace.m_bInUse;
		packet.shortRange = trace.m_bShortRange;
		packet.friendly = trace.m_bFriendly;
		packet.coordBlipAppearance = trace.m_nCoordBlipAppearance;
		packet.size = trace.m_nBlipSize;
		builder.AddPacket(CPacketsID::CREATE_STATIC_BLIP, &packet, sizeof packet);
	}
	builder.Send(ENET_PACKET_FLAG_RELIABLE);
}
