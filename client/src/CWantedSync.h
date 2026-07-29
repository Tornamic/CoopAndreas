#pragma once

namespace Packets::Players
{
class PlayerWantedLevel;
}  // namespace Packets::Players

class CWantedSync
{
public:
    static void Process();
    static void HandlePacket(Packets::Players::PlayerWantedLevel* packet);
    static void NotifyLocalCrime();
    static void RecordCrimeSource(CPed* creator, const CVector& position);
    static void RecordVehicleEntrySource(CPed* player, CVehicle* vehicle);
    static void RecordVehicleDriverSource(CPed* driver, const CVector& position);
    static void NotifyPayNSpray();
    static void NotifyPoliceBribe();
    static bool AnyRemotePlayerWantedAbove(uint8_t wantedLevel);
    static void InjectHooks();
};