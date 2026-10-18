#pragma once

#include "stdafx.h"
#include <CObject.h>
#include <unordered_map>
#include <vector>

// breakable and movable map objects (lamp posts, petrol pumps, bins, boxes) look the same for every player. An object
// is made from its dummy when a player comes within 80 m and turned back (as new) when it goes away, so the server
// keeps what happened to it and streams it to the players around; a new object gets the known state before it is
// rendered. Damage is reported by whoever sees it, the motion by the one who pushed the object
class CWorldObjectSync
{
public:
    static void Reset();
    static void Process();
    static void HandlePacket(Packets::World::WorldObjectStates* pPacket);
    static void HandlePacket(Packets::World::WorldObjectMotion* pPacket);

private:
    static constexpr uint32_t MOTION_INTERVAL = 100;
    // the local physics of an object isn't ours: until the mover's rest state (or its motion stops coming), and a bit
    // after it while ours settles. A background window of the mover may run a frame a second
    static constexpr uint32_t REMOTE_MOTION_TIME = 5000;
    static constexpr uint32_t REMOTE_REST_TIME = 1000;

    struct SSlot
    {
        int ref = 0;  // CPools::GetObjectRef + 1, 0 - no map object in the slot
        CObject* pObject = nullptr;
        Packets::World::WorldObjectKey key;
        uint8_t flags = 0;  // the damage flags last seen
        bool bMoving = false;
        bool bMover = false;  // we pushed it: we send its motion and where it stopped
        uint32_t lastMotionTime = 0;
        uint32_t remoteUntil = 0;
        Packets::World::WorldObjectTransform original;  // the dummy's: the object or the dummy may be gone later
        Packets::World::WorldObjectTransform lastTransform;
    };

    using KeyMap = std::unordered_map<Packets::World::WorldObjectKey, int, Packets::World::WorldObjectKeyHash>;
    using StateMap = std::unordered_map<Packets::World::WorldObjectKey, Packets::World::WorldObjectState,
        Packets::World::WorldObjectKeyHash>;

    static SSlot* FindSlot(const Packets::World::WorldObjectKey& key);
    static void OnObjectAppeared(SSlot& slot);
    static void OnObjectGone(SSlot& slot);
    static void ProcessSlot(SSlot& slot, uint32_t now);
    static bool ShouldBecomeMover(CObject* pObject);

    static void Apply(SSlot& slot, const Packets::World::WorldObjectState& state, bool bLive, bool bTransform);
    static void SendState(const SSlot& slot, bool bTransform, uint8_t flags);
    static void Remember(const Packets::World::WorldObjectKey& key, bool bTransform,
        const Packets::World::WorldObjectState& state);

    static std::vector<SSlot> ms_slots;  // by the object pool index
    static KeyMap ms_slotByKey;
    static StateMap ms_states;  // what the server told us, and our own changes
};
