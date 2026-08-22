#pragma once

namespace Packets
{
namespace World
{
class GameCheatSync;
}  // namespace World
}  // namespace Packets

class CCheatSync
{
public:
    static void Init();
    static void Process();
    static void OnLocalCheat(uint8_t cheatId);
    static void HandlePacket(Packets::World::GameCheatSync* packet);
};