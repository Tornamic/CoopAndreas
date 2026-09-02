#include "stdafx.h"
#include "CPickupSession.h"
#include "CNetworkPlayer.h"
#include "CPacketFactory.h"
#include "network/packets/world.h"
#include <unordered_set>
#include <vector>

namespace
{
    std::vector<Packets::World::PickupCollected> s_collectedPickups;
    std::unordered_set<uint64_t> s_seenKeys;

    uint64_t MakeCollectedKey(const Packets::World::PickupCollected& collected)
    {
        return (static_cast<uint64_t>(collected.pos_x) << 48) | (static_cast<uint64_t>(collected.pos_y) << 32)
            | (static_cast<uint64_t>(collected.pos_z) << 16) | collected.modelId;
    }

    bool IsValidCollected(const Packets::World::PickupCollected& collected)
    {
        return collected.modelId != 0;
    }
}

void CPickupSession::AddCollected(const Packets::World::PickupCollected& collected)
{
    if (!IsValidCollected(collected))
    {
        return;
    }

    const uint64_t key = MakeCollectedKey(collected);
    if (!s_seenKeys.insert(key).second)
    {
        return;
    }

    s_collectedPickups.push_back(collected);
}

void CPickupSession::FillUpdateAll(Packets::World::UpdateAllPickups& packet)
{
    packet.count = 0;
    for (const Packets::World::PickupCollected& collected : s_collectedPickups)
    {
        if (packet.count >= Packets::World::UpdateAllPickups::MAX_PICKUPS)
        {
            break;
        }

        Packets::World::UpdateAllPickups::Entry& entry = packet.entries[packet.count++];
        entry.pickupSlot = collected.pickupSlot;
        entry.pos_x = collected.pos_x;
        entry.pos_y = collected.pos_y;
        entry.pos_z = collected.pos_z;
        entry.modelId = collected.modelId;
    }
}

void CPickupSession::SendToPlayer(CNetworkPlayer* target)
{
    if (target == nullptr || s_collectedPickups.empty())
    {
        return;
    }

    Packets::World::UpdateAllPickups packet{};
    FillUpdateAll(packet);
    GetPacketFactory().Send(packet, target);
}
