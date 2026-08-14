#pragma once

namespace Packets
{
namespace World
{
class GameWeatherTime;
class LightningEvent;
}  // namespace World
}  // namespace Packets

class CWeatherSync
{
public:
    static void Reset();
    static bool HasNetState();

    static void ApplyNetStateToLocal();

    static void SyncCurrentState();

    static void HandlePacket(Packets::World::GameWeatherTime* pGameWeatherTime);

    static void HandleLightningEvent(Packets::World::LightningEvent* pLightningEvent);

    static void Process();

    static void ServerTimeRecalculated(server_time_t serverTime);
};