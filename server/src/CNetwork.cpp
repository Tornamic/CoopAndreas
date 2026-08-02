#include "CNetworkPlayerManager.h"
#include "CPacketFactory.h"
#include "logger.h"
#include "network/packet.h"
#include "network/packets/system.h"
#include "serialize.h"
#include "stdafx.h"
#include <network/packets/vehicles.h>
#include <network/packets/peds.h>
#include <network/packets/scripts.h>

static void UpdatePlayerPings()
{
    static server_time_t nLastUpdate = 0;
    constexpr server_time_t PING_UPDATE_INTERVAL = 1000;

    if (g_serverTime - nLastUpdate < PING_UPDATE_INTERVAL || CNetworkPlayerManager::m_pPlayers.empty())
    {
        return;
    }

    nLastUpdate = g_serverTime;

    Packets::System::PlayerPing playerPing{};
    for (int nPlayerId = 0; nPlayerId < Config::MAX_SERVER_PLAYERS; nPlayerId++)
    {
        CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(nPlayerId);
        if (pNetworkPlayer == nullptr)
        {
            continue;
        }

        playerPing.ping[playerPing.playerCount++] = pNetworkPlayer->m_nPing;
    }

    GetPacketFactory().SendToAll(playerPing);
}

bool CNetwork::Init(unsigned short port)
{
    if (enet_initialize() != 0)  // try to init enet
    {
        printf("[ERROR] : ENET_INIT FAILED TO INITIALIZE\n");
        return false;
    }

    ENetAddress address;

    address.host = ENET_HOST_ANY;  // bind server ip
    address.port = port;           // bind server port

    // TODO: `ConfigManager::GetConfigMaxPlayers`
    ENetHost* pENetHost =
        enet_host_create(&address, Config::MAX_SERVER_PLAYERS, (int)ePacketChannel::COUNT, 0, 0);  // create enet host

    if (pENetHost == nullptr)
    {
        printf("[ERROR] : ENET_UDP_SERVER_SOCKET FAILED TO CREATE\n");
        return false;
    }

    printf("[!] : Server started on port %d\n", port);

    ENetEvent eNetEvent{};
    while (true)  // waiting for event
    {
        CServerTime::Update();
        UpdatePlayerPings();

        while (enet_host_service(pENetHost, &eNetEvent, 1) > 0)
        {
            switch (eNetEvent.type)
            {
                case ENET_EVENT_TYPE_CONNECT:
                {
                    CNetwork::HandlePeerConnected(eNetEvent);
                    break;
                }
                case ENET_EVENT_TYPE_RECEIVE:
                {
                    CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(eNetEvent.peer);
                    if (pNetworkPlayer != nullptr)
                    {
                        GetPacketFactory().Receive(
                            eNetEvent.packet->data, eNetEvent.packet->dataLength, pNetworkPlayer);
                    }
                    else
                    {
                        GetPacketFactory().ReceivePacketNoAuth_ENet(
                            eNetEvent.packet->data, eNetEvent.packet->dataLength, eNetEvent.peer);
                    }

                    enet_packet_destroy(eNetEvent.packet);
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT:
                {
                    CNetwork::HandlePlayerDisconnected(eNetEvent);
                    break;
                }
                case ENET_EVENT_TYPE_NONE:
                    break;
            }
        }
    }

    enet_host_destroy(pENetHost);
    enet_deinitialize();
    printf("[!] : Server Shutdown (ENET_DEINITIALIZE)\n");
    return 0;
}

void CNetwork::HandlePeerConnected(ENetEvent& event)
{
    printf("[Game] : A new client connected from %i.%i.%i.%i:%u.\n", event.peer->address.host & 0xFF,
        (event.peer->address.host >> 8) & 0xFF, (event.peer->address.host >> 16) & 0xFF,
        (event.peer->address.host >> 24) & 0xFF, event.peer->address.port);

    enet_peer_timeout(event.peer, 0, 30000, 60000);  // timeoutLimit, timeoutMinimum, timeoutMaximum
}

void CNetwork::HandlePlayerDisconnected(ENetEvent& event)
{
    CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(event.peer);

    if (pNetworkPlayer == nullptr)
    {
        return;
    }

    CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(pNetworkPlayer->m_nVehicleId);

    if (vehicle != nullptr)
    {
        vehicle->m_pPlayers[pNetworkPlayer->m_nSeatId] = nullptr;
    }

    if (Packets::Scripts::g_pLastEnExPlayerOwner == pNetworkPlayer)
    {
        Packets::Scripts::g_pLastEnExPlayerOwner = nullptr;
    }

    CNetworkPedManager::RemoveAllHostedAndNotify(pNetworkPlayer);
    CNetworkVehicleManager::RemoveAllHostedAndNotify(pNetworkPlayer);

    CNetworkPlayerManager::Remove(pNetworkPlayer);

    Packets::System::PlayerDisconnected playerDisconnected{};
    playerDisconnected.payload.playerid = pNetworkPlayer->m_iPlayerId;
    playerDisconnected.payload.reason = Packets::System::PlayerDisconnected::DISCONNECTION_REASON_NOTHING;
    GetPacketFactory().SendToAll(playerDisconnected);

    printf("[Game] : %i Disconnected.\n", pNetworkPlayer->m_iPlayerId);

    CNetworkPlayerManager::AssignHostToFirstPlayer();
}

void CNetwork::HandlePlayerConnected(ENetPeer* pENetPeer, Packets::System::PlayerConnected& playerConnected)
{
#ifndef _DEBUG
    for (auto* pNetworkPlayer : CNetworkPlayerManager::m_pPlayers)
    {
        if (strncmp(pNetworkPlayer->m_Name, playerConnected.payload.name, sizeof(pNetworkPlayer->m_Name)) == 0)
        {
            Packets::System::PlayerDisconnected playerDisconnected{};
            playerDisconnected.payload.playerid = -1;
            playerDisconnected.payload.reason = Packets::System::PlayerDisconnected::DISCONNECTION_REASON_NAME_TAKEN;
            GetPacketFactory().SendPacketNoAuth_ENet(playerDisconnected, pENetPeer);
            enet_peer_disconnect_later(pENetPeer, 0);
            return;
        }
    }
#endif

    uint32_t packedVersion = semver_parse(COOPANDREAS_VERSION, nullptr);
    char buffer[23];
    semver_t playerVersion;
    semver_unpack(playerConnected.payload.version, &playerVersion);
    semver_to_string(&playerVersion, buffer, sizeof(buffer));
    buffer[22] = '\0';

    if (packedVersion != playerConnected.payload.version)
    {
        Packets::System::PlayerDisconnected playerDisconnected{};
        playerDisconnected.payload.playerid = -1;
        playerDisconnected.payload.reason = Packets::System::PlayerDisconnected::DISCONNECTION_REASON_VERSION_MISMATCH;
        playerDisconnected.payload.version = packedVersion;
        GetPacketFactory().SendPacketNoAuth_ENet(playerDisconnected, pENetPeer);
        enet_peer_disconnect_later(pENetPeer, packedVersion);
        return;
    }

    int freeId = CNetworkPlayerManager::GetFreeID();
    CNetworkPlayer* pNewNetworkPlayer = new CNetworkPlayer(pENetPeer, freeId);
    strcpy_s(pNewNetworkPlayer->m_Name, playerConnected.payload.name);
    CNetworkPlayerManager::Add(pNewNetworkPlayer);

    logger::info("freeId %d name %s version %s", freeId, playerConnected.payload.name, buffer);

    // Send the NEW player TO OLD players
    playerConnected.payload.playerid = freeId;
    GetPacketFactory().SendToAll(playerConnected, pNewNetworkPlayer);

    // Let the new player know his id
    Packets::System::PlayerHandshake playerHandshake{};
    playerHandshake.yourid = freeId;
    GetPacketFactory().Send(playerHandshake, pNewNetworkPlayer);

    // Send OLD players TO the NEW one
    Packets::System::PlayerConnected oldPlayerConnected{};
    oldPlayerConnected.payload.isAlreadyConnected = true;

    for (auto* pNetworkPlayer : CNetworkPlayerManager::m_pPlayers)
    {
        if (pNetworkPlayer->m_iPlayerId == freeId)
        {
            continue;
        }
        oldPlayerConnected.payload.playerid = pNetworkPlayer->m_iPlayerId;
        strcpy_s(oldPlayerConnected.payload.name, pNetworkPlayer->m_Name);
        GetPacketFactory().Send(oldPlayerConnected, pNewNetworkPlayer);
    }

    for (auto i : CNetworkPlayerManager::m_pPlayers)
    {
        if (i->m_iPlayerId == freeId)
            continue;

        if (i->m_ucSyncFlags.bStatsModified)
        {
            Packets::Players::PlayerStats statsPacket{};
            statsPacket.playerid = i->m_iPlayerId;
            for (size_t j = 0; j < ARRAY_SIZE(statsPacket.stats); j++)
            {
                statsPacket.stats[j] = i->m_afStats[j];
            }
            GetPacketFactory().Send(statsPacket, pNewNetworkPlayer);
        }

        if (i->m_ucSyncFlags.bClothesModified)
        {
            Packets::Players::RebuildPlayer rebuildPacket{};
            rebuildPacket.playerid = i->m_iPlayerId;
            rebuildPacket.clothesDesc = i->m_pPedClothesDesc;
            GetPacketFactory().Send(rebuildPacket, pNewNetworkPlayer);
        }

        if (i->m_waypointState.place)
        {
            i->m_waypointState.playerid = i->m_iPlayerId;
            GetPacketFactory().Send(i->m_waypointState, pNewNetworkPlayer);
        }
    }

    for (auto i : CNetworkVehicleManager::m_pVehicles)
    {
        Packets::Vehicles::VehicleSpawn vehicleSpawnPacket{};
        vehicleSpawnPacket.vehicleid = i->m_nVehicleId;
        vehicleSpawnPacket.modelid = i->m_nModelId;
        vehicleSpawnPacket.pos = i->m_vecPosition;
        vehicleSpawnPacket.rot = static_cast<float>(
            i->m_vecRotation.z * (3.141592f / 180.0f));  // convert to radians TODO(v0.3.1-alpha): is this wrong?
        vehicleSpawnPacket.color1 = i->m_nPrimaryColor;
        vehicleSpawnPacket.color2 = i->m_nSecondaryColor;
        GetPacketFactory().Send(vehicleSpawnPacket, pNewNetworkPlayer);

        Packets::Vehicles::VehicleDamage vehicleDamagePacket{};
        vehicleDamagePacket.vehicleid = i->m_nVehicleId;
        vehicleDamagePacket.damageManager = i->m_damageManager;
        GetPacketFactory().Send(vehicleDamagePacket, pNewNetworkPlayer);

        for (int component : i->m_pComponents)
        {
            Packets::Vehicles::VehicleComponentAdd vehicleComponentAdd{};
            vehicleComponentAdd.vehicleid = i->m_nVehicleId;
            vehicleComponentAdd.componentid = component;
            GetPacketFactory().Send(vehicleComponentAdd, pNewNetworkPlayer);
        }
    }

    for (auto i : CNetworkPedManager::m_pPeds)
    {
        Packets::Peds::PedSpawn packet{};
        packet.pedid = i->m_nPedId;
        packet.modelId = i->m_nModelId;
        packet.pos = i->m_vecPos;
        packet.pedType = i->m_nPedType;
        packet.createdBy = i->m_nCreatedBy;
        snprintf(packet.specialModelName, sizeof(packet.specialModelName), "%s", i->m_szSpecialModelName);
        GetPacketFactory().Send(packet, pNewNetworkPlayer);
    }

     if (Packets::Scripts::g_pLastEnExPlayerOwner)
     {
         if (std::find(CNetworkPlayerManager::m_pPlayers.begin(), CNetworkPlayerManager::m_pPlayers.end(),
             Packets::Scripts::g_pLastEnExPlayerOwner) != CNetworkPlayerManager::m_pPlayers.end())
         {
             GetPacketFactory().Send(Packets::Scripts::g_lastEnExData, pNewNetworkPlayer);
         }
    }

    CNetworkPlayerManager::AssignHostToFirstPlayer();
}

void CNetwork::SendPacketNoAuth_ENet(ENetPeer* pENetPeer, const uint8_t* data, int dataSize,
    ePacketChannel packetChannel, ePacketReliability packetReliability)
{
    uint32_t eNetPacketFlags = 0;
    if (packetReliability == ePacketReliability::RELIABLE)
    {
        eNetPacketFlags |= ENET_PACKET_FLAG_RELIABLE;
    }
    else if (packetReliability == ePacketReliability::UNRELIABLE)
    {
        // ...
    }

    ENetPacket* pENetPacket = enet_packet_create(data, dataSize, eNetPacketFlags);
    if (pENetPacket != nullptr)
    {
        enet_peer_send(pENetPeer, static_cast<uint8_t>(packetChannel), pENetPacket);
    }
}

void CNetwork::SendPacket(CNetworkPlayer* pNetworkPlayer, const uint8_t* data, int dataSize,
    ePacketChannel packetChannel, ePacketReliability packetReliability)
{
    assert(pNetworkPlayer != nullptr);

    uint32_t eNetPacketFlags = 0;
    if (packetReliability == ePacketReliability::RELIABLE)
    {
        eNetPacketFlags |= ENET_PACKET_FLAG_RELIABLE;
    }
    else if (packetReliability == ePacketReliability::UNRELIABLE)
    {
        // ...
    }

    ENetPacket* pENetPacket = enet_packet_create(data, dataSize, eNetPacketFlags);
    if (pENetPacket != nullptr)
    {
        enet_peer_send(pNetworkPlayer->m_pPeer, static_cast<uint8_t>(packetChannel), pENetPacket);
    }
}

void CNetwork::SendPacketToAll(const uint8_t* data, int dataSize, ePacketChannel packetChannel,
    ePacketReliability packetReliability, CNetworkPlayer* pNetworkPlayerToIgnore)
{
    uint32_t eNetPacketFlags = 0;
    if (packetReliability == ePacketReliability::RELIABLE)
    {
        eNetPacketFlags |= ENET_PACKET_FLAG_RELIABLE;
    }
    else if (packetReliability == ePacketReliability::UNRELIABLE)
    {
        // ...
    }

    ENetPacket* pENetPacket = enet_packet_create(data, dataSize, eNetPacketFlags);
    if (pENetPacket != nullptr)
    {
        for (auto* pNetworkPlayer : CNetworkPlayerManager::m_pPlayers)
        {
            if (pNetworkPlayer != pNetworkPlayerToIgnore)
            {
                enet_peer_send(pNetworkPlayer->m_pPeer, static_cast<uint8_t>(packetChannel), pENetPacket);
            }
        }
    }
}
