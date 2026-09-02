#pragma once

class CNetworkPlayer;

namespace Packets::World
{
class PickupCollected;
class UpdateAllPickups;
}

class CPickupSession
{
public:
	static void AddCollected(const Packets::World::PickupCollected& collected);
	static void SendToPlayer(CNetworkPlayer* target);
	static void FillUpdateAll(Packets::World::UpdateAllPickups& packet);
};
