#include "stdafx.h"
#include "CNetwork.h"
#include "CPackets.h"
#include "CNetworkPlayerManager.h"
#include "CNetworkVehicleManager.h"
#include "CNetworkVehicle.h"
#include "CNetworkPedManager.h"
#include "CNetworkPed.h"

std::vector<CPacketListener*> CNetwork::m_packetListeners;

bool CNetwork::Init(unsigned short port)
{
    // init packet listeners
    CNetwork::InitListeners();

    if (enet_initialize() != 0) // try to init enet
    {
        printf("init failed\n");
        return false;
    }

    ENetAddress address;

    address.host = ENET_HOST_ANY; // bind server ip
    address.port = port; // bind server port

    ENetHost* server = enet_host_create(&address, MAX_SERVER_PLAYERS, 2, 0, 0); // create enet host

    if (server == NULL)
    {
        printf("creating failed\n");
        return false;
    }

    printf("Server stared on port %d\n", port);

    ENetEvent event;
    while (true) // waiting for event
    {
        enet_host_service(server, &event, 1);
        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                CNetwork::HandlePlayerConnected(event);
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {
                CNetwork::HandlePacketReceive(event);
                enet_packet_destroy(event.packet);

                char buffer[100];
                sprintf_s(buffer, "Recv %d Sent %d", server->totalReceivedPackets, server->totalSentPackets);
                SetConsoleTitleA(buffer);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                CNetwork::HandlePlayerDisconnected(event);
                break;
            }
        }
    }

    enet_host_destroy(server);
    enet_deinitialize();
    printf("Destroyed\n");
    return 0;
}

void CNetwork::InitListeners()
{
    CNetwork::AddListener(ePacketType::PLAYER_ONFOOT, CPackets::PlayerOnFoot::Handle);
    CNetwork::AddListener(ePacketType::PLAYER_BULLET_SHOT, CPackets::PlayerBulletShot::Handle);
    CNetwork::AddListener(ePacketType::PLAYER_PLACE_WAYPOINT, CPackets::PlayerPlaceWaypoint::Handle);
    CNetwork::AddListener(ePacketType::PLAYER_GET_NAME, CPackets::PlayerGetName::Handle);
    CNetwork::AddListener(ePacketType::ADD_EXPLOSION, CPackets::AddExplosion::Handle);
    CNetwork::AddListener(ePacketType::VEHICLE_SPAWN, CPackets::VehicleSpawn::Handle);
    CNetwork::AddListener(ePacketType::VEHICLE_REMOVE, CPackets::VehicleRemove::Handle);
    CNetwork::AddListener(ePacketType::VEHICLE_ENTER, CPackets::VehicleEnter::Handle);
    CNetwork::AddListener(ePacketType::VEHICLE_EXIT, CPackets::VehicleExit::Handle);
    CNetwork::AddListener(ePacketType::VEHICLE_DRIVER_UPDATE, CPackets::VehicleDriverUpdate::Handle);
    CNetwork::AddListener(ePacketType::VEHICLE_IDLE_UPDATE, CPackets::VehicleIdleUpdate::Handle);
    CNetwork::AddListener(ePacketType::VEHICLE_DAMAGE, CPackets::VehicleDamage::Handle);
    CNetwork::AddListener(ePacketType::VEHICLE_COMPONENT_ADD, CPackets::VehicleComponentAdd::Handle);
    CNetwork::AddListener(ePacketType::VEHICLE_COMPONENT_REMOVE, CPackets::VehicleComponentRemove::Handle);
    CNetwork::AddListener(ePacketType::VEHICLE_PASSENGER_UPDATE, CPackets::VehiclePassengerUpdate::Handle);
    CNetwork::AddListener(ePacketType::PLAYER_CHAT_MESSAGE, CPackets::PlayerChatMessage::Handle);
    CNetwork::AddListener(ePacketType::PED_SPAWN, CPackets::PedSpawn::Handle);
    CNetwork::AddListener(ePacketType::PED_REMOVE, CPackets::PedRemove::Handle);
    CNetwork::AddListener(ePacketType::PED_ONFOOT, CPackets::PedOnFoot::Handle);
    CNetwork::AddListener(ePacketType::GAME_WEATHER_TIME, CPackets::GameWeatherTime::Handle);
    CNetwork::AddListener(ePacketType::PLAYER_KEY_SYNC, CPackets::PlayerKeySync::Handle);
    CNetwork::AddListener(ePacketType::PED_ADD_TASK, CPackets::PedAddTask::Handle);
    CNetwork::AddListener(ePacketType::PED_DRIVER_UPDATE, CPackets::PedDriverUpdate::Handle);
}

void CNetwork::SendPacket(ENetPeer* peer, unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag)
{
    // 2 == sizeof(unsigned short)

    // packet size `id + data`
    size_t packetSize = 2 + dataSize;

    // create buffer
    char* packetData = new char[packetSize];

    // copy id
    memcpy(packetData, &id, 2);

    // copy data
    memcpy(packetData + 2, data, dataSize);

    // create packet
    ENetPacket* packet = enet_packet_create(packetData, packetSize, flag & ENET_PACKET_FLAG_NO_ALLOCATE);

    delete[] packetData;

    // send packet
    enet_peer_send(peer, 0, packet);
}

void CNetwork::SendPacketToAll(unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag, ENetPeer* dontShareWith = nullptr)
{
    size_t packetSize = 2 + dataSize;
    char* packetData = new char[packetSize];
    memcpy(packetData, &id, 2);
    memcpy(packetData + 2, data, dataSize);
    ENetPacket* packet = enet_packet_create(packetData, packetSize, flag & ENET_PACKET_FLAG_NO_ALLOCATE);

    delete[] packetData;

    auto& players = CNetworkPlayerManager::Instance().GetEntities();

    for (int i = 0; i != players.size(); i++)
    {
        if (players[i]->m_pPeer != dontShareWith)
        {
            enet_peer_send(players[i]->m_pPeer, 0, packet);
        }
    }
}

void CNetwork::SendPacketStreamFor(unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag, CNetworkPlayer* player)
{

}

void CNetwork::SendPacketRawToAll(void* data, size_t dataSize, ENetPacketFlag flag, ENetPeer* dontShareWith = nullptr)
{
    ENetPacket* packet = enet_packet_create(data, dataSize, flag & ENET_PACKET_FLAG_NO_ALLOCATE);

    auto& players = CNetworkPlayerManager::Instance().GetEntities();

    for (int i = 0; i != players.size(); i++)
    {
        if (players[i]->m_pPeer != dontShareWith)
        {
            enet_peer_send(players[i]->m_pPeer, 0, packet);
        }
    }
}

void CNetwork::HandlePlayerConnected(ENetEvent& event)
{
    printf("A new client connected from %i.%i.%i.%i:%u.\n", 
        event.peer->address.host & 0xFF, 
        (event.peer->address.host >> 8) & 0xFF, 
        (event.peer->address.host >> 16) & 0xFF, 
        (event.peer->address.host >> 24) & 0xFF, 
        event.peer->address.port);

    // set player disconnection timeout
    enet_peer_timeout(event.peer, 5000, 3000, 5000); //timeoutLimit, timeoutMinimum, timeoutMaximum

    // create new player and send to all players

    auto& playerManager = CNetworkPlayerManager::Instance();

    // get free id
    uint16_t freeId = playerManager.GetFreeId();

    // create new player instance
    CNetworkPlayer* player = new CNetworkPlayer(freeId, event.peer);

    player->GetSyncData().m_vecPosition = CVector(1488.0f, 69.69f, 1337.0f);
    // add player to list
    playerManager.Add(player);

    // create PlayerConnected packet struct
    CPackets::PlayerConnected packet =
    {
        freeId // id
    };

    // send to all
    CNetwork::SendPacketToAll(ePacketType::PLAYER_CONNECTED, &packet, sizeof CPackets::PlayerConnected, ENET_PACKET_FLAG_RELIABLE, event.peer);

    // send PlayerConnected packets for a new player
    for (auto i : playerManager.GetEntities())
    {
        if (i->GetId() == freeId)
            continue;

        packet =
        {
            i->GetId()
        };

        CNetwork::SendPacket(event.peer, ePacketType::PLAYER_CONNECTED, &packet, sizeof CPackets::PlayerConnected, ENET_PACKET_FLAG_RELIABLE);

        CPackets::PlayerGetName getNamePacket{};
        getNamePacket.m_nPlayerId = i->GetId();
        strcpy_s(getNamePacket.m_aName, i->GetName());
        CNetwork::SendPacket(event.peer, ePacketType::PLAYER_GET_NAME, &getNamePacket, sizeof CPackets::PlayerGetName, ENET_PACKET_FLAG_RELIABLE);
    }

    for (auto i : CNetworkVehicleManager::Instance().GetEntities())
    {
        auto syncData = i->GetSyncData();

        CPackets::VehicleSpawn vehicleSpawnPacket
        { 
            i->GetId(),
            i->GetModelId(),
            syncData.m_vecPosition,
            syncData.m_vecRotation.z * (3.141592f / 180), // convert to radians
            syncData.m_nPrimaryColor,
            syncData.m_nSecondaryColor
        };

        CNetwork::SendPacket(event.peer, ePacketType::VEHICLE_SPAWN, &vehicleSpawnPacket, sizeof vehicleSpawnPacket, ENET_PACKET_FLAG_RELIABLE);
        
        bool modifiedDamageStatus = false;

        for (unsigned char j = 0; j < 23; j++)
        {
            if (i->m_damageManager_padding[j])
            {
                modifiedDamageStatus = true; 
                break;
            }
        }

        if (modifiedDamageStatus)
        {
            CPackets::VehicleDamage vehicleDamagePacket{};
            vehicleDamagePacket.m_nVehicleId = i->GetId();
            memcpy(&vehicleDamagePacket.damageManager_padding, i->m_damageManager_padding, 23);
            CNetwork::SendPacket(event.peer, ePacketType::VEHICLE_DAMAGE, &vehicleDamagePacket, sizeof vehicleDamagePacket, ENET_PACKET_FLAG_RELIABLE);
        }

        for (int component : i->m_aComponents) 
        {
            CPackets::VehicleComponentAdd vehicleComponentAddPacket{};
            vehicleComponentAddPacket.m_nVehicleId = i->GetId();
            vehicleComponentAddPacket.m_nComponentId = component;
            CNetwork::SendPacket(event.peer, ePacketType::VEHICLE_COMPONENT_ADD, &vehicleComponentAddPacket, sizeof vehicleComponentAddPacket, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    for (auto i : CNetworkPedManager::Instance().GetEntities())
    {
        CPackets::PedSpawn packet{};
        packet.m_nPedId = i->GetId();
        packet.m_nModelId = i->GetModelId();
        packet.m_vecPosition = i->GetSyncData().m_vecPosition;
        packet.m_nPedType = i->GetPedType();
        packet.m_nCreatedBy = i->GetCreatedBy();
        CNetwork::SendPacket(event.peer, ePacketType::PED_SPAWN, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    }

    CPackets::PlayerHandshake handshakePacket = { freeId };
    CNetwork::SendPacket(event.peer, ePacketType::PLAYER_HANDSHAKE, &handshakePacket, sizeof handshakePacket, ENET_PACKET_FLAG_RELIABLE);

    playerManager.AssignHostToFirstPlayer();
}

void CNetwork::HandlePlayerDisconnected(ENetEvent& event)
{
    auto& playerManager = CNetworkPlayerManager::Instance();

    CNetworkPlayer* player = playerManager.Get(event.peer);
    
    CNetworkVehicle* vehicle = player->GetVehicle();

    if (vehicle != nullptr)
    {
        vehicle->m_aOccupants[player->GetSeatId()] = nullptr;
    }

    playerManager.Remove(player);

    CPackets::PlayerDisconnected packet =
    {
        player->GetId()
    };

    CNetwork::SendPacketToAll(ePacketType::PLAYER_DISCONNECTED, &packet, sizeof CPackets::PlayerDisconnected, ENET_PACKET_FLAG_UNSEQUENCED, event.peer);

    printf("%i disconnected.\n", player->GetId());

    playerManager.AssignHostToFirstPlayer();
}

void CNetwork::HandlePacketReceive(ENetEvent& event)
{
    // get packet id
    unsigned short id;
    memcpy(&id, event.packet->data, 2);

    // get data
    char* data = new char[event.packet->dataLength - 2];
    memcpy(data, event.packet->data + 2, event.packet->dataLength - 2);

    // call listener's callback by id
    for (size_t i = 0; i < m_packetListeners.size(); i++)
    {
        if (m_packetListeners[i]->m_iPacketID == id)
        {
            m_packetListeners[i]->m_callback(event.peer, data, event.packet->dataLength - 2);
        }
    }
    
    delete[] data;
}

void CNetwork::AddListener(unsigned short id, void(*callback)(ENetPeer*, void*, int))
{
    CPacketListener* listener = new CPacketListener(id, callback);
    m_packetListeners.push_back(listener);
}