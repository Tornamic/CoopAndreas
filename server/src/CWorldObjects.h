#pragma once

#include "network/packets/world.h"
#include <unordered_map>
#include <vector>

class CNetworkPlayer;

// the map objects players broke or moved (lamp posts, petrol pumps, bins...). A player gets the ones around it as it
// comes near, so the place looks the same for someone who arrives later. Nobody near for a while - the object is
// forgotten: every client has long turned it back into its dummy, as the single player game does
class CWorldObjects
{
public:
    // a dummy becomes an object 80 m from the player (CPopulation::ManageDummy), the state is there long before that
    static constexpr float STREAM_DISTANCE = 250.0f;
    static constexpr float KEEP_DISTANCE = 200.0f;
    static constexpr server_time_t FORGET_AFTER = 60000;

    static void HandleStates(Packets::World::WorldObjectStates* pPacket, CNetworkPlayer* pSender);
    static void HandleMotion(Packets::World::WorldObjectMotion* pPacket, CNetworkPlayer* pSender);
    static void Update();
    static void OnPlayerDisconnected(CNetworkPlayer* pNetworkPlayer);

private:
    struct SSentVersion
    {
        CNetworkPlayer* pPlayer;
        uint32_t version;
    };

    struct SEntry
    {
        Packets::World::WorldObjectState state;
        CVector position;  // the dummy's, for the distances
        uint32_t version = 0;
        server_time_t lastNearTime = 0;
        std::vector<SSentVersion> sent;  // which version every player has
    };

    using EntryMap = std::unordered_map<Packets::World::WorldObjectKey, SEntry, Packets::World::WorldObjectKeyHash>;

    static uint32_t GetSentVersion(const SEntry& entry, CNetworkPlayer* pNetworkPlayer);
    static void SetSentVersion(SEntry& entry, CNetworkPlayer* pNetworkPlayer, uint32_t version);
    static void SendForgotten(const Packets::World::WorldObjectKey& key, const SEntry& entry);

    static inline EntryMap ms_entries;
    static inline uint32_t ms_nextVersion = 1;
};
