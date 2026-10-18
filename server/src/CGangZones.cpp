#include "stdafx.h"
#include "CGangZones.h"
#include "CNetworkPlayer.h"

void CGangZones::HandleUpdate(Packets::World::GangZonesUpdate* pUpdate, CNetworkPlayer* pSender)
{
    if (!pSender->m_bIsHost)
    {
        return;
    }

    for (int i = 0; i < pUpdate->count; i++)
    {
        const auto& zone = pUpdate->zones[i];
        memcpy(ms_aStrength[zone.index], zone.strength, sizeof(zone.strength));
        ms_abKnown[zone.index] = true;
    }

    GetPacketFactory().SendToAll(*pUpdate, pSender);
}

void CGangZones::HandleSettings(Packets::World::GangWarsSettings* pSettings, CNetworkPlayer* pSender)
{
    if (!pSender->m_bIsHost)
    {
        return;
    }

    ms_settings = *pSettings;
    ms_bHasSettings = true;
    GetPacketFactory().SendToAll(*pSettings, pSender);
}

void CGangZones::SendState(CNetworkPlayer* pTarget)
{
    Packets::World::GangZonesUpdate update{};
    for (uint16_t i = 0; i < Packets::World::MAX_GANG_ZONES; i++)
    {
        if (ms_abKnown[i])
        {
            auto& zone = update.zones[update.count++];
            zone.index = i;
            memcpy(zone.strength, ms_aStrength[i], sizeof(zone.strength));
        }
    }

    if (update.count > 0)
    {
        GetPacketFactory().Send(update, pTarget);
    }

    if (ms_bHasSettings)
    {
        GetPacketFactory().Send(ms_settings, pTarget);
    }
}

void CGangZones::OnPlayerDisconnected(CNetworkPlayer* pNetworkPlayer)
{
    // the last one left: the next session may be another game, its host brings its own table
    if (CNetworkPlayerManager::m_pPlayers.size() <= 1)
    {
        memset(ms_abKnown, 0, sizeof(ms_abKnown));
        ms_bHasSettings = false;
    }
}
