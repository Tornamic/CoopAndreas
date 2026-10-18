#include "stdafx.h"
#include "CGangZones.h"
#include "CNetworkPlayer.h"

void CGangZones::HandleUpdate(Packets::World::GangZonesUpdate* pUpdate, CNetworkPlayer* pSender)
{
    // the owner of a war changes the zone it is fought for
    if (!pSender->m_bIsHost && pSender != ms_pWarOwner)
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

void CGangZones::HandleWarState(Packets::World::GangWarState* pState, CNetworkPlayer* pSender)
{
    // a war that lost the race ends there and says so, nobody mirrors it
    if ((ms_pWarOwner != nullptr && ms_pWarOwner != pSender) || (ms_pWarOwner == nullptr && !pState->active))
    {
        return;
    }

    pState->owner = pSender->m_playerId;
    ms_pWarOwner = pState->active ? pSender : nullptr;
    ms_warState = *pState;
    GetPacketFactory().SendToAll(*pState, pSender);
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

    if (ms_pWarOwner != nullptr)
    {
        GetPacketFactory().Send(ms_warState, pTarget);
    }
}

void CGangZones::OnPlayerDisconnected(CNetworkPlayer* pNetworkPlayer)
{
    if (pNetworkPlayer == ms_pWarOwner)
    {
        // its attackers are removed with everything it synced
        ms_warState.active = false;
        ms_warState.pedCount = 0;
        ms_warState.result = Packets::World::GangWarState::RESULT_ABORTED;
        GetPacketFactory().SendToAll(ms_warState, pNetworkPlayer);
        ms_pWarOwner = nullptr;
    }

    // the last one left: the next session may be another game, its host brings its own table
    if (CNetworkPlayerManager::m_pPlayers.size() <= 1)
    {
        memset(ms_abKnown, 0, sizeof(ms_abKnown));
        ms_bHasSettings = false;
    }
}
