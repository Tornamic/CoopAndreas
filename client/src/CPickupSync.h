#pragma once

namespace Packets::World
{
class PickupCollected;
class PickupSpawn;
class UpdateAllPickups;
}

class CPickupSync
{
public:
	static void NotifyCollected(uint16_t pickupSlot, const CVector* posOverride = nullptr);
	static void NotifyCollectedAt(const CVector& pos, uint16_t modelId);
	static void ApplyCollected(const Packets::World::PickupCollected& packet, bool applyCollectibleReward = true);
	static void NotifyPickupSpawned(
		const CVector& position, uint16_t modelId, uint8_t pickupType, uint16_t ammo, bool isEmpty);
	static void NotifyMoneySpawned(const CVector& position, int amount);
	static void NotifyJetpackPickedUp(const CVector& position);
	static void ApplySpawned(const Packets::World::PickupSpawn& packet);
	static void HandleUpdateAll(const Packets::World::UpdateAllPickups& packet);
	static bool IsApplyingRemote();
	static bool ShouldSyncSpawnType(uint8_t pickupType);
};
