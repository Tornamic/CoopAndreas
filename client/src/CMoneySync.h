#pragma once

namespace Packets::World
{
class GameMoneySync;
}  // namespace Packets::World

class CMoneySync
{
public:
    static void SyncCurrentState();
    static void HandlePacket(Packets::World::GameMoneySync* packet);
};