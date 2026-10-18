#include "stdafx.h"
#include "CWorldObjects.h"
#include "CNetworkPlayer.h"
#include "CNetworkPlayerManager.h"
#include "CServerTime.h"

using namespace Packets::World;

// the server doesn't link the plugin-sdk CVector methods
static bool IsNear(CNetworkPlayer* pNetworkPlayer, const CVector& position, float distance)
{
    if (!pNetworkPlayer->m_bHasPosition)
    {
        return false;
    }

    float dx = pNetworkPlayer->m_vecPosition.x - position.x;
    float dy = pNetworkPlayer->m_vecPosition.y - position.y;
    float dz = pNetworkPlayer->m_vecPosition.z - position.z;
    return dx * dx + dy * dy + dz * dz < distance * distance;
}

uint32_t CWorldObjects::GetSentVersion(const SEntry& entry, CNetworkPlayer* pNetworkPlayer)
{
    for (const auto& sent : entry.sent)
    {
        if (sent.pPlayer == pNetworkPlayer)
        {
            return sent.version;
        }
    }
    return 0;
}

void CWorldObjects::SetSentVersion(SEntry& entry, CNetworkPlayer* pNetworkPlayer, uint32_t version)
{
    for (auto& sent : entry.sent)
    {
        if (sent.pPlayer == pNetworkPlayer)
        {
            sent.version = version;
            return;
        }
    }
    entry.sent.push_back({pNetworkPlayer, version});
}

void CWorldObjects::SendForgotten(const WorldObjectKey& key, const SEntry& entry)
{
    WorldObjectStates packet{};
    packet.count = 1;
    packet.entries[0].key = key;

    for (const auto& sent : entry.sent)
    {
        GetPacketFactory().Send(packet, sent.pPlayer);
    }
}

void CWorldObjects::HandleStates(WorldObjectStates* pPacket, CNetworkPlayer* pSender)
{
    struct SChange
    {
        WorldObjectStates::Entry entry;
        CVector position;
        std::vector<CNetworkPlayer*> holders;  // a restored object: who has it, near or not
    };
    std::vector<SChange> changes;

    for (int i = 0; i < pPacket->count; i++)
    {
        const auto& reported = pPacket->entries[i];
        auto it = ms_entries.find(reported.key);
        bool bKnown = it != ms_entries.end();

        // the damage only grows: two players may report the same pump, each from its own copy of the explosion
        WorldObjectState state = bKnown ? it->second.state : WorldObjectState{};
        state.flags |= reported.state.flags & WorldObjectState::DAMAGE_MASK;
        if (reported.hasTransform)
        {
            state.flags = (state.flags & ~WorldObjectState::MOVED) | (reported.state.flags & WorldObjectState::MOVED);
            state.transform = reported.state.transform;
        }

        bool bTransformChanged = reported.hasTransform && (state.flags & WorldObjectState::MOVED);
        if (bKnown && state.flags == it->second.state.flags && !bTransformChanged)
        {
            continue;
        }

        if (!bKnown && state.flags == 0)
        {
            continue;
        }

        SChange change{};
        change.entry.key = reported.key;
        change.entry.hasTransform = reported.hasTransform || state.flags == 0;
        change.entry.state = state;
        change.position = reported.key.GetPosition();

        if (state.flags == 0)
        {
            // pushed back to where it stands on the map: as if nothing happened to it
            for (const auto& sent : it->second.sent)
            {
                change.holders.push_back(sent.pPlayer);
            }
            ms_entries.erase(it);
        }
        else
        {
            SEntry& entry = ms_entries[reported.key];
            if (!bKnown)
            {
                entry.position = change.position;
                entry.lastNearTime = g_serverTime;
            }
            entry.state = state;
            entry.version = ms_nextVersion++;
            SetSentVersion(entry, pSender, entry.version);
        }

        changes.push_back(change);
    }

    if (changes.empty())
    {
        return;
    }

    for (auto pNetworkPlayer : CNetworkPlayerManager::m_pPlayers)
    {
        if (pNetworkPlayer == pSender)
        {
            continue;
        }

        WorldObjectStates packet{};
        packet.live = true;
        for (const auto& change : changes)
        {
            bool bHolder = std::find(change.holders.begin(), change.holders.end(), pNetworkPlayer) !=
                           change.holders.end();
            if (!bHolder && !IsNear(pNetworkPlayer, change.position, STREAM_DISTANCE))
            {
                continue;  // gets it from Update when it comes near
            }

            packet.entries[packet.count++] = change.entry;

            auto it = ms_entries.find(change.entry.key);
            if (it != ms_entries.end())
            {
                SetSentVersion(it->second, pNetworkPlayer, it->second.version);
            }
        }

        if (packet.count > 0)
        {
            GetPacketFactory().Send(packet, pNetworkPlayer);
        }
    }
}

void CWorldObjects::HandleMotion(WorldObjectMotion* pPacket, CNetworkPlayer* pSender)
{
    pPacket->mover = pSender->m_playerId;

    CVector position = pPacket->key.GetPosition();
    for (auto pNetworkPlayer : CNetworkPlayerManager::m_pPlayers)
    {
        if (pNetworkPlayer != pSender && IsNear(pNetworkPlayer, position, STREAM_DISTANCE))
        {
            GetPacketFactory().Send(*pPacket, pNetworkPlayer);
        }
    }
}

void CWorldObjects::Update()
{
    static server_time_t nLastUpdate = 0;
    constexpr server_time_t UPDATE_INTERVAL = 500;

    if (g_serverTime - nLastUpdate < UPDATE_INTERVAL)
    {
        return;
    }
    nLastUpdate = g_serverTime;

    for (auto it = ms_entries.begin(); it != ms_entries.end();)
    {
        SEntry& entry = it->second;
        bool bNear = std::any_of(CNetworkPlayerManager::m_pPlayers.begin(), CNetworkPlayerManager::m_pPlayers.end(),
            [&](CNetworkPlayer* pNetworkPlayer) { return IsNear(pNetworkPlayer, entry.position, KEEP_DISTANCE); });

        if (bNear)
        {
            entry.lastNearTime = g_serverTime;
        }
        else if (g_serverTime - entry.lastNearTime > FORGET_AFTER)
        {
            SendForgotten(it->first, entry);
            it = ms_entries.erase(it);
            continue;
        }
        ++it;
    }

    // streaming: the objects around every player it doesn't have yet, or has an older state of
    for (auto pNetworkPlayer : CNetworkPlayerManager::m_pPlayers)
    {
        if (!pNetworkPlayer->m_bHasPosition)
        {
            continue;
        }

        WorldObjectStates packet{};
        for (auto& [key, entry] : ms_entries)
        {
            if (GetSentVersion(entry, pNetworkPlayer) == entry.version ||
                !IsNear(pNetworkPlayer, entry.position, STREAM_DISTANCE))
            {
                continue;
            }

            auto& packetEntry = packet.entries[packet.count++];
            packetEntry.key = key;
            packetEntry.hasTransform = true;
            packetEntry.state = entry.state;
            SetSentVersion(entry, pNetworkPlayer, entry.version);

            if (packet.count == WorldObjectStates::MAX_ENTRIES)
            {
                GetPacketFactory().Send(packet, pNetworkPlayer);
                packet.count = 0;
            }
        }

        if (packet.count > 0)
        {
            GetPacketFactory().Send(packet, pNetworkPlayer);
        }
    }
}

void CWorldObjects::OnPlayerDisconnected(CNetworkPlayer* pNetworkPlayer)
{
    // the last one left: the next session may be another game
    if (CNetworkPlayerManager::m_pPlayers.size() <= 1)
    {
        ms_entries.clear();
        return;
    }

    for (auto& [key, entry] : ms_entries)
    {
        auto& sent = entry.sent;
        sent.erase(std::remove_if(sent.begin(), sent.end(),
                       [pNetworkPlayer](const SSentVersion& s) { return s.pPlayer == pNetworkPlayer; }),
            sent.end());
    }
}
