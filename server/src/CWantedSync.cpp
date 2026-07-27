#include "stdafx.h"
#include "CWantedSync.h"
#include "network/packets/players.h"

namespace
{
constexpr uint32_t BRIBE_DEDUPLICATION_WINDOW = 1500;
constexpr uint32_t VEHICLE_WANTED_SETTLE_WINDOW = 1500;

uint8_t ClampWantedLevel(uint8_t level)
{
    return std::min<uint8_t>(level, 6);
}

Packets::Players::PlayerWantedLevel BuildWantedPacket(
    CNetworkPlayer* player, bool crimeCommitted, Packets::Players::eWantedSyncReason reason)
{
    Packets::Players::PlayerWantedLevel packet{};
    packet.playerid = player->m_iPlayerId;
    packet.wantedLevel = player->m_nWantedLevel;
    packet.bPoliceEngaged = player->m_bPoliceEngaged;
    packet.bCrimeCommitted = crimeCommitted;
    packet.reason = reason;
    return packet;
}

CNetworkVehicle* GetValidatedVehicle(CNetworkPlayer* player)
{
    if (!player || player->m_nVehicleId < 0 || player->m_nSeatId < 0 || player->m_nSeatId > 7)
    {
        return nullptr;
    }

    CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(player->m_nVehicleId);
    if (!vehicle || vehicle->m_pPlayers[player->m_nSeatId] != player)
    {
        player->RemoveFromVehicle();
        return nullptr;
    }

    return vehicle;
}

void SanitizeVehicleOccupants(CNetworkVehicle* vehicle)
{
    for (int seat = 0; seat < 8; ++seat)
    {
        CNetworkPlayer* occupant = vehicle->m_pPlayers[seat];
        if (!occupant)
        {
            continue;
        }

        if (occupant->m_nVehicleId != vehicle->m_nVehicleId || occupant->m_nSeatId != seat)
        {
            vehicle->m_pPlayers[seat] = nullptr;
        }
    }
}

uint32_t GetServerTime()
{
    return static_cast<uint32_t>(g_serverTime);
}

CNetworkPlayer* GetVehicleWantedAuthority(CNetworkVehicle* vehicle)
{
    for (auto* occupant : vehicle->m_pPlayers)
    {
        if (occupant)
        {
            return occupant;
        }
    }
    return nullptr;
}

uint8_t GetHighestVehicleWantedLevel(CNetworkVehicle* vehicle)
{
    uint8_t highestWantedLevel = vehicle->m_nSharedWantedLevel;
    for (auto* occupant : vehicle->m_pPlayers)
    {
        if (occupant)
        {
            highestWantedLevel = std::max(highestWantedLevel, occupant->m_nWantedLevel);
        }
    }
    return highestWantedLevel;
}

bool IsAnyVehicleOccupantPoliceEngaged(CNetworkVehicle* vehicle)
{
    for (auto* occupant : vehicle->m_pPlayers)
    {
        if (occupant && occupant->m_bPoliceEngaged)
        {
            return true;
        }
    }
    return false;
}

bool IsVehicleWantedStateSettling(CNetworkVehicle* vehicle)
{
    return vehicle->m_nWantedStateSetAt != 0 &&
           GetServerTime() - vehicle->m_nWantedStateSetAt < VEHICLE_WANTED_SETTLE_WINDOW;
}
}  // namespace

void CWantedSync::BroadcastState(CNetworkPlayer* player, CNetworkPlayer* playerToIgnore, bool crimeCommitted,
    Packets::Players::eWantedSyncReason reason)
{
    auto packet = BuildWantedPacket(player, crimeCommitted, reason);
    GetPacketFactory().SendToAll(packet, playerToIgnore);
}

void CWantedSync::SendStateTo(CNetworkPlayer* player, CNetworkPlayer* recipient)
{
    auto packet = BuildWantedPacket(player, false, Packets::Players::eWantedSyncReason::PERSONAL);
    GetPacketFactory().Send(packet, recipient);
}

void CWantedSync::ApplyVehicleWantedState(CNetworkVehicle* vehicle, uint8_t wantedLevel, bool policeEngaged,
    Packets::Players::eWantedSyncReason reason, bool forceBroadcast)
{
    wantedLevel = ClampWantedLevel(wantedLevel);
    policeEngaged = wantedLevel > 0 && policeEngaged;
    if (vehicle->m_nSharedWantedLevel != wantedLevel)
    {
        vehicle->m_nWantedStateSetAt = GetServerTime();
    }
    vehicle->m_nSharedWantedLevel = wantedLevel;
    vehicle->m_bSharedPoliceEngaged = policeEngaged;

    for (auto* occupant : vehicle->m_pPlayers)
    {
        if (!occupant)
        {
            continue;
        }

        bool stateChanged = occupant->m_nWantedLevel != wantedLevel || occupant->m_bPoliceEngaged != policeEngaged;
        occupant->m_nWantedLevel = wantedLevel;
        occupant->m_bPoliceEngaged = policeEngaged;
        if (stateChanged || forceBroadcast)
        {
            BroadcastState(occupant, nullptr, false, reason);
        }
    }
}

void CWantedSync::HandlePlayerUpdate(Packets::Players::PlayerWantedLevel* packet, CNetworkPlayer* sender)
{
    uint8_t incomingWantedLevel = ClampWantedLevel(packet->wantedLevel);
    bool incomingPoliceEngaged = incomingWantedLevel > 0 && packet->bPoliceEngaged;
    CNetworkVehicle* vehicle = GetValidatedVehicle(sender);
    if (!vehicle)
    {
        sender->m_nWantedLevel = incomingWantedLevel;
        sender->m_bPoliceEngaged = incomingPoliceEngaged;
    }
    else
    {
        SanitizeVehicleOccupants(vehicle);

        uint8_t highestWantedLevel = GetHighestVehicleWantedLevel(vehicle);
        if (highestWantedLevel > vehicle->m_nSharedWantedLevel)
        {
            bool policeEngaged = vehicle->m_bSharedPoliceEngaged || IsAnyVehicleOccupantPoliceEngaged(vehicle);
            ApplyVehicleWantedState(
                vehicle, highestWantedLevel, policeEngaged, Packets::Players::eWantedSyncReason::VEHICLE_SHARE, false);
            vehicle->m_nWantedStateSetAt = GetServerTime();
        }

        CNetworkPlayer* authority = GetVehicleWantedAuthority(vehicle);
        bool settling = IsVehicleWantedStateSettling(vehicle);
        bool crimeEscalation = packet->bCrimeCommitted && incomingWantedLevel > vehicle->m_nSharedWantedLevel;

        if (incomingWantedLevel > vehicle->m_nSharedWantedLevel && (!settling || crimeEscalation))
        {
            bool policeEngaged = vehicle->m_bSharedPoliceEngaged || incomingPoliceEngaged;
            ApplyVehicleWantedState(
                vehicle, incomingWantedLevel, policeEngaged, Packets::Players::eWantedSyncReason::VEHICLE_SHARE, false);
        }
        else if (sender == authority && incomingWantedLevel < vehicle->m_nSharedWantedLevel && !settling)
        {
            ApplyVehicleWantedState(vehicle, incomingWantedLevel, incomingPoliceEngaged,
                Packets::Players::eWantedSyncReason::VEHICLE_SHARE, false);
        }
        else if (sender == authority && incomingWantedLevel == vehicle->m_nSharedWantedLevel &&
                 incomingPoliceEngaged != vehicle->m_bSharedPoliceEngaged)
        {
            ApplyVehicleWantedState(vehicle, incomingWantedLevel, incomingPoliceEngaged,
                Packets::Players::eWantedSyncReason::VEHICLE_SHARE, false);
        }

        bool senderNeedsCorrection = incomingWantedLevel != vehicle->m_nSharedWantedLevel ||
                                     incomingPoliceEngaged != vehicle->m_bSharedPoliceEngaged;
        sender->m_nWantedLevel = vehicle->m_nSharedWantedLevel;
        sender->m_bPoliceEngaged = vehicle->m_bSharedPoliceEngaged;
        if (senderNeedsCorrection)
        {
            BroadcastState(sender, nullptr, false, Packets::Players::eWantedSyncReason::VEHICLE_SHARE);
        }
    }

    packet->playerid = sender->m_iPlayerId;
    packet->wantedLevel = sender->m_nWantedLevel;
    packet->bPoliceEngaged = sender->m_bPoliceEngaged;
    packet->reason = Packets::Players::eWantedSyncReason::PERSONAL;
    GetPacketFactory().SendToAll(*packet, sender);
}

void CWantedSync::EscalateVehicle(CNetworkVehicle* vehicle)
{
    if (!vehicle)
    {
        return;
    }

    SanitizeVehicleOccupants(vehicle);
    uint8_t highestWantedLevel = GetHighestVehicleWantedLevel(vehicle);
    bool policeEngaged =
        highestWantedLevel > 0 && (vehicle->m_bSharedPoliceEngaged || IsAnyVehicleOccupantPoliceEngaged(vehicle));
    ApplyVehicleWantedState(
        vehicle, highestWantedLevel, policeEngaged, Packets::Players::eWantedSyncReason::VEHICLE_SHARE, false);

    // Vehicle entry or a driver change can make an occupant the decay authority before its client
    // has received the shared state, so briefly reject stale state packets after membership changes.
    vehicle->m_nWantedStateSetAt = GetServerTime();
}

void CWantedSync::HandleVehicleAction(Packets::Players::VehicleWantedAction* packet, CNetworkPlayer* sender)
{
    CNetworkVehicle* vehicle = GetValidatedVehicle(sender);
    if (!vehicle)
    {
        return;
    }

    SanitizeVehicleOccupants(vehicle);

    switch (packet->action)
    {
        case Packets::Players::eVehicleWantedAction::CLEAR:
            ApplyVehicleWantedState(vehicle, 0, false, Packets::Players::eWantedSyncReason::VEHICLE_CLEAR, true);
            vehicle->m_nWantedStateSetAt = GetServerTime();
            break;

        case Packets::Players::eVehicleWantedAction::BRIBE:
        {
            uint32_t now = GetServerTime();
            if (vehicle->m_nLastWantedBribeAt != 0 && now - vehicle->m_nLastWantedBribeAt < BRIBE_DEDUPLICATION_WINDOW)
            {
                return;
            }
            vehicle->m_nLastWantedBribeAt = now;

            uint8_t wantedLevel = GetHighestVehicleWantedLevel(vehicle);
            if (wantedLevel > 0)
            {
                --wantedLevel;
            }
            bool policeEngaged =
                wantedLevel > 0 && (vehicle->m_bSharedPoliceEngaged || IsAnyVehicleOccupantPoliceEngaged(vehicle));
            ApplyVehicleWantedState(
                vehicle, wantedLevel, policeEngaged, Packets::Players::eWantedSyncReason::VEHICLE_BRIBE, true);
            vehicle->m_nWantedStateSetAt = now;
            break;
        }
    }
}

void CWantedSync::ClearPlayer(CNetworkPlayer* player)
{
    player->m_nWantedLevel = 0;
    player->m_bPoliceEngaged = false;
    BroadcastState(player, nullptr, false, Packets::Players::eWantedSyncReason::RESPAWN);
}