#include "stdafx.h"
#include "CNetworkStaticBlip.h"
#include "CEntryExit.h"
#include <CEntryExitManager.h>

void CNetworkStaticBlip::Create(const Packets::Blips::StaticBlipsSnapshot& packet)
{
    for (int i = 0; i < MAX_RADAR_TRACES; i++)
    {
        auto& trace = CRadar::ms_RadarTrace[i];

        if ((trace.m_nBlipType == BLIP_CONTACTPOINT || trace.m_nBlipType == BLIP_COORD) &&
            IsAllowedSyncingRadarSprite(static_cast<eRadarSprite>(trace.m_nRadarSprite)))
        {
            CRadar::ClearActualBlip(i);
        }
    }

    for (size_t i = 0; i < packet.countBlips; i++)
    {
        const Packets::Blips::_StaticBlipPayload& blipState = packet.blips[i];

        assert(IsAllowedSyncingRadarSprite(static_cast<eRadarSprite>(blipState.sprite)));

        // CChat::AddMessage("CNetworkMissionMarker::Create({%.2f %.2f %.2f}, %d, %d)", position.x, position.y,
        // position.z, sprite, display);

        int blip = CRadar::SetCoordBlip(
            static_cast<eBlipType>(blipState.type ? eBlipType::BLIP_COORD : eBlipType::BLIP_CONTACTPOINT),
            blipState.position, 0, static_cast<eBlipDisplay>(blipState.display), nullptr);
        CRadar::SetBlipSprite(blip, blipState.sprite);
        CRadar::ChangeBlipDisplay(blip, static_cast<eBlipDisplay>(blipState.display));

        if (const auto index = CRadar::GetActualBlipArrayIndex(blip); index != -1)
        {
            CRadar::ms_RadarTrace[index].m_bShortRange = blipState.shortRange;
            CRadar::ms_RadarTrace[index].m_bFriendly = blipState.friendly;
            CRadar::ms_RadarTrace[index].m_nCoordBlipAppearance = blipState.coordBlipAppearance;
            CRadar::ms_RadarTrace[index].m_nBlipSize = blipState.size;
            CRadar::ms_RadarTrace[index].m_nColour = blipState.color;
        }
    }
}

void CNetworkStaticBlip::Send()
{
    Packets::Blips::StaticBlipsSnapshot packet{};
    packet.countBlips = 0;
    for (int i = 0; i < MAX_RADAR_TRACES; i++)
    {
        auto& trace = CRadar::ms_RadarTrace[i];

        if ((trace.m_nBlipType != eBlipType::BLIP_CONTACTPOINT && trace.m_nBlipType != eBlipType::BLIP_COORD) ||
            !IsAllowedSyncingRadarSprite(static_cast<eRadarSprite>(trace.m_nRadarSprite)))
        {
            continue;
        }

        Packets::Blips::_StaticBlipPayload& blipState = packet.blips[packet.countBlips];

        if (trace.m_pEntryExit)
        {
            auto& rect = trace.m_pEntryExit->m_recEntrance;
            blipState.position = CVector(
                (rect.right + rect.left) * 0.5f, (rect.bottom + rect.top) * 0.5f, trace.m_pEntryExit->m_fEntranceZ);
        }
        else
        {
            blipState.position = trace.m_vecPos;
        }

        blipState.display = trace.m_nBlipDisplay;
        blipState.sprite = trace.m_nRadarSprite;
        blipState.type = trace.m_nBlipType == eBlipType::BLIP_COORD;
        blipState.trackingBlip = trace.m_bInUse;
        blipState.shortRange = trace.m_bShortRange;
        blipState.friendly = trace.m_bFriendly;
        blipState.coordBlipAppearance = trace.m_nCoordBlipAppearance;
        blipState.size = trace.m_nBlipSize;
        blipState.color = trace.m_nColour;

        ++packet.countBlips;
    }
    GetPacketFactory().Send(packet);
}
