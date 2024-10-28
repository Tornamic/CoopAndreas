#include "stdafx.h"

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
    CNetwork::AddListener(CPacketsID::PLAYER_ONFOOT, CPackets::PlayerOnFoot::Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_BULLET_SHOT, CPackets::PlayerBulletShot::Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_PLACE_WAYPOINT, CPackets::PlayerPlaceWaypoint::Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_GET_NAME, CPackets::PlayerGetName::Handle);
    CNetwork::AddListener(CPacketsID::ADD_EXPLOSION, CPackets::AddExplosion::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_SPAWN, CPackets::VehicleSpawn::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_REMOVE, CPackets::VehicleRemove::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_ENTER, CPackets::VehicleEnter::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_EXIT, CPackets::VehicleExit::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_DRIVER_UPDATE, CPackets::VehicleDriverUpdate::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_IDLE_UPDATE, CPackets::VehicleIdleUpdate::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_DAMAGE, CPackets::VehicleDamage::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_COMPONENT_ADD, CPackets::VehicleComponentAdd::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_COMPONENT_REMOVE, CPackets::VehicleComponentRemove::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_PASSENGER_UPDATE, CPackets::VehiclePassengerUpdate::Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_CHAT_MESSAGE, CPackets::PlayerChatMessage::Handle);
    CNetwork::AddListener(CPacketsID::PED_SPAWN, CPackets::PedSpawn::Handle);
    CNetwork::AddListener(CPacketsID::PED_REMOVE, CPackets::PedRemove::Handle);
    CNetwork::AddListener(CPacketsID::PED_ONFOOT, CPackets::PedOnFoot::Handle);
    CNetwork::AddListener(CPacketsID::GAME_WEATHER_TIME, CPackets::GameWeatherTime::Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_KEY_SYNC, CPackets::PlayerKeySync::Handle);
    CNetwork::AddListener(CPacketsID::PED_ADD_TASK, CPackets::PedAddTask::Handle);
    CNetwork::AddListener(CPacketsID::PED_DRIVER_UPDATE, CPackets::PedDriverUpdate::Handle);
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

    for (int i = 0; i != CPlayerManager::m_pPlayers.size(); i++)
    {
        if (CPlayerManager::m_pPlayers[i]->m_pPeer != dontShareWith)
        {
            enet_peer_send(CPlayerManager::m_pPlayers[i]->m_pPeer, 0, packet);
        }
    }
}

void CNetwork::SendPacketStreamFor(unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag, CPlayer* player)
{

}

void CNetwork::SendPacketRawToAll(void* data, size_t dataSize, ENetPacketFlag flag, ENetPeer* dontShareWith = nullptr)
{
    ENetPacket* packet = enet_packet_create(data, dataSize, flag & ENET_PACKET_FLAG_NO_ALLOCATE);

    for (int i = 0; i != CPlayerManager::m_pPlayers.size(); i++)
    {
        if (CPlayerManager::m_pPlayers[i]->m_pPeer != dontShareWith)
        {
            enet_peer_send(CPlayerManager::m_pPlayers[i]->m_pPeer, 0, packet);
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

    // get free id
    int freeId = CPlayerManager::GetFreeID();

    // create new player instance
    CPlayer* player = new CPlayer(event.peer, freeId);

    // add player to list
    CPlayerManager::Add(player);

    // create PlayerConnected packet struct
    CPackets::PlayerConnected packet =
    {
        freeId // id
    };

    // send to all
    CNetwork::SendPacketToAll(CPacketsID::PLAYER_CONNECTED, &packet, sizeof CPackets::PlayerConnected, ENET_PACKET_FLAG_RELIABLE, event.peer);

    // send PlayerConnected packets for a new player
    for (auto i : CPlayerManager::m_pPlayers)
    {
        if (i->m_iPlayerId == freeId)
            continue;

        packet =
        {
            i->m_iPlayerId
        };

        CNetwork::SendPacket(event.peer, CPacketsID::PLAYER_CONNECTED, &packet, sizeof CPackets::PlayerConnected, ENET_PACKET_FLAG_RELIABLE);

        CPackets::PlayerGetName getNamePacket{};
        getNamePacket.playerid = i->m_iPlayerId;
        strcpy_s(getNamePacket.name, i->m_Name);
        CNetwork::SendPacket(event.peer, CPacketsID::PLAYER_GET_NAME, &getNamePacket, sizeof CPackets::PlayerGetName, ENET_PACKET_FLAG_RELIABLE);
    }

    for (auto i : CVehicleManager::m_pVehicles)
    {
        CPackets::VehicleSpawn vehicleSpawnPacket
        { 
            i->m_nVehicleId,
            i->m_nModelId,
            i->m_vecPosition,
            i->m_vecRotation.z * (3.141592 / 180), // convert to radians
            i->m_nPrimaryColor,
            i->m_nSecondaryColor
        };

        CNetwork::SendPacket(event.peer, CPacketsID::VEHICLE_SPAWN, &vehicleSpawnPacket, sizeof vehicleSpawnPacket, ENET_PACKET_FLAG_RELIABLE);
        
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
            vehicleDamagePacket.vehicleid = i->m_nVehicleId;
            memcpy(&vehicleDamagePacket.damageManager_padding, i->m_damageManager_padding, 23);
            CNetwork::SendPacket(event.peer, CPacketsID::VEHICLE_DAMAGE, &vehicleDamagePacket, sizeof vehicleDamagePacket, ENET_PACKET_FLAG_RELIABLE);
        }

        for (int component : i->m_pComponents) 
        {
            CPackets::VehicleComponentAdd vehicleComponentAddPacket{};
            vehicleComponentAddPacket.vehicleid = i->m_nVehicleId;
            vehicleComponentAddPacket.componentid = component;
            CNetwork::SendPacket(event.peer, CPacketsID::VEHICLE_COMPONENT_ADD, &vehicleComponentAddPacket, sizeof vehicleComponentAddPacket, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    for (auto i : CPedManager::m_pPeds)
    {
        CPackets::PedSpawn packet{};
        packet.pedid = i->m_nPedId;
        packet.modelId = i->m_nModelId;
        packet.pos = i->m_vecPos;
        packet.pedType = i->m_nPedType;
        packet.createdBy = i->m_nCreatedBy;
        CNetwork::SendPacket(event.peer, CPacketsID::PED_SPAWN, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    }

    CPackets::PlayerHandshake handshakePacket = { freeId };
    CNetwork::SendPacket(event.peer, CPacketsID::PLAYER_HANDSHAKE, &handshakePacket, sizeof handshakePacket, ENET_PACKET_FLAG_RELIABLE);

    CPlayerManager::AssignHostToFirstPlayer();
}

void CNetwork::HandlePlayerDisconnected(ENetEvent& event)
{
    // disconnect player

    // find player instance by enetpeer
    CPlayer* player = CPlayerManager::GetPlayer(event.peer);
    
    CVehicle* vehicle = CVehicleManager::GetVehicle(player->m_nVehicleId);

    if (vehicle != nullptr)
    {
        vehicle->m_pPlayers[player->m_nSeatId] = nullptr;
    }

    // remove
    CPlayerManager::Remove(player);

    // create PlayerDisconnected packet struct
    CPackets::PlayerDisconnected packet =
    {
        player->m_iPlayerId // id
    };

    // send to all
    CNetwork::SendPacketToAll(CPacketsID::PLAYER_DISCONNECTED, &packet, sizeof CPackets::PlayerDisconnected, ENET_PACKET_FLAG_UNSEQUENCED, event.peer);

    printf("%i disconnected.\n", player->m_iPlayerId);

    CPlayerManager::AssignHostToFirstPlayer();
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