#pragma once

#include "network/packets/players.h"

class CNetworkPlayer;
class CNetworkVehicle;

class CWantedSync
{
public:
    static void HandlePlayerUpdate(Packets::Players::PlayerWantedLevel* packet, CNetworkPlayer* sender);
    static void HandleVehicleAction(Packets::Players::VehicleWantedAction* packet, CNetworkPlayer* sender);
    static void EscalateVehicle(CNetworkVehicle* vehicle);
    static void ClearPlayer(CNetworkPlayer* player);
    static void SendStateTo(CNetworkPlayer* player, CNetworkPlayer* recipient);

private:
    static void ApplyVehicleWantedState(CNetworkVehicle* vehicle, uint8_t wantedLevel, bool policeEngaged,
        Packets::Players::eWantedSyncReason reason, bool forceBroadcast);
    static void BroadcastState(CNetworkPlayer* player, CNetworkPlayer* playerToIgnore, bool crimeCommitted,
        Packets::Players::eWantedSyncReason reason);
};