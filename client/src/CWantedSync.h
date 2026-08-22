#pragma once

namespace Packets
{
namespace World
{
class GameWantedSync;
}  // namespace World
}  // namespace Packets

class CWantedSync
{
public:
    static void SyncCurrentState();
    static void HandlePacket(Packets::World::GameWantedSync* packet);
};