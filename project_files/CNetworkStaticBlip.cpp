#include "stdafx.h"
#include "CNetworkStaticBlip.h"

void CNetworkStaticBlip::Create(CVector& position, eRadarSprite sprite, eBlipDisplay display, eBlipType type, bool trackingBlip, bool shortRange)
{
	assert(type == eBlipType::BLIP_CONTACTPOINT || type == eBlipType::BLIP_COORD);
	assert(IsAllowedSyncingRadarSprite(sprite));

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
				trace.m_bInUse = false;
			}
		}
		ms_bMassUpdateJustReceived = false;
	}

	int blip = CRadar::SetCoordBlip(type, position, 0, display, nullptr);
	CRadar::SetBlipSprite(blip, sprite);
	CRadar::ChangeBlipDisplay(blip, display);

	if(const auto index = CRadar::GetActualBlipArrayIndex(blip); index != -1)
	{
		CRadar::ms_RadarTrace[index].m_bShortRange = shortRange;
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

		packet.position = trace.m_vecPos;
		packet.display = trace.m_nBlipDisplay;
		packet.sprite = trace.m_nRadarSprite;
		packet.type = trace.m_nBlipType == eBlipType::BLIP_COORD;
		packet.trackingBlip = trace.m_bInUse;
		packet.shortRange = trace.m_bShortRange;
		builder.AddPacket(CPacketsID::CREATE_STATIC_BLIP, &packet, sizeof packet);
	}
	builder.Send(ENET_PACKET_FLAG_RELIABLE);
}

/*void CNetworkStaticBlip::Remove(CVector& position)
{
	CChat::AddMessage("CNetworkMissionMarker::Remove({%.2f %.2f %.2f})", position.x, position.y, position.z);

	SMissionMarker* marker = nullptr;

	for (auto& m : CNetworkStaticBlip::ms_vMissionMarkers)
	{
		if ((m.m_vecPosition - position).Magnitude() < 0.5f)
		{
			marker = &m;
			break;
		}
	}

	if (marker)
	{
		CRadar::ClearBlip(marker->m_nBlipHandle);

		auto it = std::find(CNetworkStaticBlip::ms_vMissionMarkers.begin(), CNetworkStaticBlip::ms_vMissionMarkers.end(), *marker);
		if (it != CNetworkStaticBlip::ms_vMissionMarkers.end())
		{
			CNetworkStaticBlip::ms_vMissionMarkers.erase(it);
		}
	}
}

void CNetworkStaticBlip::ClearAll()
{
	for (auto& m : CNetworkStaticBlip::ms_vMissionMarkers)
	{
		CRadar::ClearBlip(m.m_nBlipHandle);

		auto it = std::find(CNetworkStaticBlip::ms_vMissionMarkers.begin(), CNetworkStaticBlip::ms_vMissionMarkers.end(), m);
		if (it != CNetworkStaticBlip::ms_vMissionMarkers.end())
		{
			CNetworkStaticBlip::ms_vMissionMarkers.erase(it);
		}
	}
}*/
