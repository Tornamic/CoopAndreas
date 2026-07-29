#include "network/packet_types.h"
#include "stdafx.h"
#include "network/packet_handler.h"
#include "network/packets/vehicles.h"
#include "CWantedSync.h"

namespace
{
constexpr uint32_t VEHICLE_EXIT_SYNC_GUARD = 2000;
}  // namespace

PACKET_HANDLER(
    ePacketType::VEHICLE_SPAWN, Packets::Vehicles::VehicleSpawn* pVehicleSpawn, CNetworkPlayer* pNetworkPlayer)
{
    pVehicleSpawn->vehicleid = CNetworkVehicleManager::GetFreeID();
    GetPacketFactory().SendToAll(*pVehicleSpawn, pNetworkPlayer);

    // send it back to the syncer of the vehicle so that he knows the id
    Packets::Vehicles::VehicleConfirm vehicleConfirmPacket{};
    vehicleConfirmPacket.tempid = pVehicleSpawn->tempid;
    vehicleConfirmPacket.vehicleid = pVehicleSpawn->vehicleid;
    GetPacketFactory().Send(vehicleConfirmPacket, pNetworkPlayer);

    CNetworkVehicle* vehicle = new CNetworkVehicle(
        pVehicleSpawn->vehicleid, pVehicleSpawn->modelid, pVehicleSpawn->pos, pVehicleSpawn->rot.m_angle);

    vehicle->m_pSyncer = pNetworkPlayer;
    vehicle->m_nPrimaryColor = pVehicleSpawn->color1;
    vehicle->m_nSecondaryColor = pVehicleSpawn->color2;
    vehicle->m_nCreatedBy = pVehicleSpawn->createdBy;

    CNetworkVehicleManager::Add(vehicle);
}

PACKET_HANDLER(
    ePacketType::VEHICLE_REMOVE, Packets::Vehicles::VehicleRemove* pVehicleRemove, CNetworkPlayer* pNetworkPlayer)
{
    if (auto vehicle = CNetworkVehicleManager::GetVehicle(pVehicleRemove->vehicleid))
    {
        if (vehicle->m_pSyncer == pNetworkPlayer)
        {
            GetPacketFactory().SendToAll(*pVehicleRemove, pNetworkPlayer);

            CNetworkVehicleManager::Remove(vehicle);
        }
        else
        {
            logger::warn("%s tried to delete someone else's vehicle", pNetworkPlayer->GetName().c_str());
        }
    }
}

PACKET_HANDLER(ePacketType::VEHICLE_IDLE_UPDATE, Packets::Vehicles::VehicleIdleUpdate* pVehicleIdleUpdate,
    CNetworkPlayer* pNetworkPlayer)
{
    if (auto vehicle = CNetworkVehicleManager::GetVehicle(pVehicleIdleUpdate->vehicleid))
    {
        if (vehicle->m_pSyncer == pNetworkPlayer)
        {
            vehicle->m_bUsedByPed = false;
            vehicle->m_vecPosition = pVehicleIdleUpdate->pos;
            vehicle->m_vecRotation = pVehicleIdleUpdate->rot;
            GetPacketFactory().SendToAll(*pVehicleIdleUpdate, pNetworkPlayer);
        }
        else
        {
            logger::warn("%s tried to update someone else's vehicle (idle)", pNetworkPlayer->GetName().c_str());
        }
    }
    else
    {
        logger::warn("%s tried to update a vehicle (idle) that didn't exist", pNetworkPlayer->GetName().c_str());
    }
}

PACKET_HANDLER(ePacketType::VEHICLE_DRIVER_UPDATE, Packets::Vehicles::VehicleDriverUpdate* pVehicleDriverUpdate,
    CNetworkPlayer* pNetworkPlayer)
{
    if (pNetworkPlayer->IsVehicleSyncGuardActive())
    {
        return;
    }

    if (auto pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pVehicleDriverUpdate->vehicleid))
    {
        bool occupantChanged = pNetworkVehicle->m_pPlayers[0] != pNetworkPlayer;
        pNetworkVehicle->SetOccupant(0, pNetworkPlayer);
        if (occupantChanged)
        {
            CWantedSync::EscalateVehicle(pNetworkVehicle);
        }
        pNetworkVehicle->m_vecPosition = pVehicleDriverUpdate->pos;
        pNetworkVehicle->m_vecRotation = pVehicleDriverUpdate->rot;
        pNetworkVehicle->m_bUsedByPed = false;
        pNetworkVehicle->ReassignSyncer(pNetworkPlayer);

        pVehicleDriverUpdate->playerid = pNetworkPlayer->m_iPlayerId;
        GetPacketFactory().SendToAll(*pVehicleDriverUpdate, pNetworkPlayer);
    }
    else
    {
        logger::warn("%s tried to update a vehicle (driver) that didn't exist", pNetworkPlayer->GetName().c_str());
    }
}

PACKET_HANDLER(
    ePacketType::VEHICLE_ENTER, Packets::Vehicles::VehicleEnter* pVehicleEnter, CNetworkPlayer* pNetworkPlayer)
{
    if (auto pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pVehicleEnter->vehicleid))
    {
        pNetworkPlayer->ClearVehicleSyncGuard();
        if (pVehicleEnter->bPassenger)
            pNetworkVehicle->SetOccupant(pVehicleEnter->seatid + 1, pNetworkPlayer);
        else
            pNetworkVehicle->SetOccupant(0, pNetworkPlayer);
        CWantedSync::EscalateVehicle(pNetworkVehicle);
        pVehicleEnter->playerid = pNetworkPlayer->m_iPlayerId;
        GetPacketFactory().SendToAll(*pVehicleEnter, pNetworkPlayer);
    }
    else
    {
        logger::warn("%s tried to enter a vehicle that didn't exist", pNetworkPlayer->GetName().c_str());
    }
}

PACKET_HANDLER(ePacketType::VEHICLE_EXIT, Packets::Vehicles::VehicleExit* pVehicleExit, CNetworkPlayer* pNetworkPlayer)
{
    pVehicleExit->playerid = pNetworkPlayer->m_iPlayerId;
    GetPacketFactory().SendToAll(*pVehicleExit, pNetworkPlayer);
    pNetworkPlayer->RemoveFromVehicle();
    pNetworkPlayer->GuardVehicleSyncFor(VEHICLE_EXIT_SYNC_GUARD);
}

PACKET_HANDLER(
    ePacketType::VEHICLE_DAMAGE, Packets::Vehicles::VehicleDamage* pVehicleDamage, CNetworkPlayer* pNetworkPlayer)
{
    if (auto pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pVehicleDamage->vehicleid))
    {
        pNetworkVehicle->m_damageManager = pVehicleDamage->damageManager;

        GetPacketFactory().SendToAll(*pVehicleDamage, pNetworkPlayer);
    }
    else
    {
        logger::warn("%s tried to damage a vehicle that didn't exist", pNetworkPlayer->GetName().c_str());
    }
}

PACKET_HANDLER(ePacketType::VEHICLE_COMPONENT_ADD, Packets::Vehicles::VehicleComponentAdd* pVehicleComponentAdd,
    CNetworkPlayer* pNetworkPlayer)
{
    if (auto pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pVehicleComponentAdd->vehicleid))
    {
        pNetworkVehicle->m_pComponents.push_back(pVehicleComponentAdd->componentid);

        GetPacketFactory().SendToAll(*pVehicleComponentAdd, pNetworkPlayer);
    }
    else
    {
        logger::warn("%s tried to sync a vehicle (comp add) that didn't exist", pNetworkPlayer->GetName().c_str());
    }
}

PACKET_HANDLER(ePacketType::VEHICLE_COMPONENT_REMOVE,
    Packets::Vehicles::VehicleComponentRemove* pVehicleComponentRemove, CNetworkPlayer* pNetworkPlayer)
{
    if (auto pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pVehicleComponentRemove->vehicleid))
    {
        auto it = std::find(pNetworkVehicle->m_pComponents.begin(), pNetworkVehicle->m_pComponents.end(),
            pVehicleComponentRemove->componentid);
        if (it != pNetworkVehicle->m_pComponents.end())
        {
            pNetworkVehicle->m_pComponents.erase(it);
        }

        GetPacketFactory().SendToAll(*pVehicleComponentRemove, pNetworkPlayer);
    }
    else
    {
        logger::warn("%s tried to sync a vehicle (comp remove) that didn't exist", pNetworkPlayer->GetName().c_str());
    }
}

PACKET_HANDLER(ePacketType::VEHICLE_PASSENGER_UPDATE,
    Packets::Vehicles::VehiclePassengerUpdate* pVehiclePassengerUpdate, CNetworkPlayer* pNetworkPlayer)
{
    if (pNetworkPlayer->IsVehicleSyncGuardActive())
    {
        return;
    }

    if (auto pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pVehiclePassengerUpdate->vehicleid))
    {
        int8_t seatId = pVehiclePassengerUpdate->seatid + 1;
        if (seatId < 1 || seatId > 7)
        {
            return;
        }

        pVehiclePassengerUpdate->playerid = pNetworkPlayer->m_iPlayerId;
        GetPacketFactory().SendToAll(*pVehiclePassengerUpdate, pNetworkPlayer);

        bool occupantChanged = pNetworkVehicle->m_pPlayers[seatId] != pNetworkPlayer;
        pNetworkVehicle->SetOccupant(seatId, pNetworkPlayer);
        if (occupantChanged)
        {
            CWantedSync::EscalateVehicle(pNetworkVehicle);
        }

        if (pNetworkVehicle->m_nCreatedBy == MISSION_VEHICLE)
        {
            return;
        }

        if (!pNetworkVehicle->m_pPlayers[0] && !pNetworkVehicle->m_bUsedByPed)  // if no driver
        {
            for (uint8_t i = 1; i < 8; i++)
            {
                if (auto pNewSyncer = pNetworkVehicle->m_pPlayers[i])
                {
                    pNetworkVehicle->ReassignSyncer(pNewSyncer);
                    break;
                }
            }
        }
    }
    else
    {
        logger::warn("%s tried to sync a vehicle (passenger) that didn't exist", pNetworkPlayer->GetName().c_str());
    }
}

PACKET_HANDLER(ePacketType::SET_VEHICLE_CREATED_BY, Packets::Vehicles::SetVehicleCreatedBy* pSetVehicleCreatedBy,
    CNetworkPlayer* pNetworkPlayer)
{
    if (pNetworkPlayer->m_bIsHost)
    {
        if (auto pNetworkVehicle = CNetworkVehicleManager::GetVehicle(pSetVehicleCreatedBy->vehicleid))
        {
            pNetworkVehicle->m_nCreatedBy = pSetVehicleCreatedBy->createdBy;
            GetPacketFactory().SendToAll(*pSetVehicleCreatedBy, pNetworkPlayer);
        }
    }
}
