
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include "../core/CPacketListener.h"
#include "../core/CPacket.h"
#include "../core/CNetwork.h"

#include "../core/CPlayerManager.h"
#include "../core/CVehicleManager.h"
#include "../core/CPedManager.h"

#include "../shared/semver.h"


std::unordered_map<unsigned short, CPacketListener*> CNetwork::m_packetListeners;

bool CNetwork::Init(unsigned short port)
{
    // init packet listeners
    CNetwork::InitListeners();

    if (enet_initialize() != 0) // try to init enet
    {
        printf("[ERROR] : ENET_INIT FAILED TO INITIALIZE\n");
        return false;
    }

    ENetAddress address;

    address.host = ENET_HOST_ANY; // bind server ip
    address.port = port; // bind server port

    ENetHost* server = enet_host_create(&address, MAX_SERVER_PLAYERS, 2, 0, 0); // create enet host

    if (server == NULL)
    {
        printf("[ERROR] : ENET_UDP_SERVER_SOCKET FAILED TO CREATE\n");
        return false;
    }

    printf("[!] : Server started on port %d\n", port);


    ENetEvent event;
    while (true) // waiting for event
    {
        enet_host_service(server, &event, 1);
        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                CNetwork::HandlePeerConnected(event);
                break;
            }
            case ENET_EVENT_TYPE_RECEIVE:
            {
                CNetwork::HandlePacketReceive(event);
                enet_packet_destroy(event.packet);
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
    printf("[!] : Server Shutdown (ENET_DEINITIALIZE)\n");
    return 0;
}

void CNetwork::InitListeners()
{
    CNetwork::AddListener(CPacketsID::PLAYER_CONNECTED, CPlayerPackets::PlayerConnected::Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_ONFOOT, CPlayerPackets::PlayerOnFoot::Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_BULLET_SHOT, CPlayerPackets::PlayerBulletShot::Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_PLACE_WAYPOINT, CPlayerPackets::PlayerPlaceWaypoint::Handle);
    CNetwork::AddListener(CPacketsID::ADD_EXPLOSION, CPlayerPackets::AddExplosion::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_SPAWN, CVehiclePackets::VehicleSpawn::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_REMOVE, CVehiclePackets::VehicleRemove::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_ENTER, CVehiclePackets::VehicleEnter::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_EXIT, CVehiclePackets::VehicleExit::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_DRIVER_UPDATE, CVehiclePackets::VehicleDriverUpdate::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_IDLE_UPDATE, CVehiclePackets::VehicleIdleUpdate::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_DAMAGE, CVehiclePackets::VehicleDamage::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_COMPONENT_ADD, CVehiclePackets::VehicleComponentAdd::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_COMPONENT_REMOVE, CVehiclePackets::VehicleComponentRemove::Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_PASSENGER_UPDATE, CVehiclePackets::VehiclePassengerUpdate::Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_CHAT_MESSAGE, CPlayerPackets::PlayerChatMessage::Handle);
    CNetwork::AddListener(CPacketsID::PED_SPAWN, CPedPackets::PedSpawn::Handle);
    CNetwork::AddListener(CPacketsID::PED_REMOVE, CPedPackets::PedRemove::Handle);
    CNetwork::AddListener(CPacketsID::PED_ONFOOT, CPedPackets::PedOnFoot::Handle);
    CNetwork::AddListener(CPacketsID::GAME_WEATHER_TIME, CPlayerPackets::GameWeatherTime::Handle); // CPlayerPacket
    CNetwork::AddListener(CPacketsID::PLAYER_KEY_SYNC, CPlayerPackets::PlayerKeySync::Handle); 
    CNetwork::AddListener(CPacketsID::PED_ADD_TASK, CPedPackets::PedAddTask::Handle);
    CNetwork::AddListener(CPacketsID::PED_DRIVER_UPDATE, CPedPackets::PedDriverUpdate::Handle);
    CNetwork::AddListener(CPacketsID::PED_SHOT_SYNC, CPedPackets::PedShotSync::Handle);
    CNetwork::AddListener(CPacketsID::PED_PASSENGER_UPDATE, CPedPackets::PedPassengerSync::Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_AIM_SYNC, CPlayerPackets::PlayerAimSync::Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_STATS, CPlayerPackets::PlayerStats::Handle);
    CNetwork::AddListener(CPacketsID::REBUILD_PLAYER, CPlayerPackets::RebuildPlayer::Handle);
    CNetwork::AddListener(CPacketsID::RESPAWN_PLAYER, CPlayerPackets::RespawnPlayer::Handle);
    CNetwork::AddListener(CPacketsID::START_CUTSCENE, CPlayerPackets::StartCutscene::Handle);
    CNetwork::AddListener(CPacketsID::SKIP_CUTSCENE, CPlayerPackets::SkipCutscene::Handle);
    CNetwork::AddListener(CPacketsID::OPCODE_SYNC, CPlayerPackets::OpCodeSync::Handle);
    CNetwork::AddListener(CPacketsID::ON_MISSION_FLAG_SYNC, CPlayerPackets::OnMissionFlagSync::Handle);
    CNetwork::AddListener(CPacketsID::UPDATE_ENTITY_BLIP, CPlayerPackets::UpdateEntityBlip::Handle);
    CNetwork::AddListener(CPacketsID::REMOVE_ENTITY_BLIP, CPlayerPackets::RemoveEntityBlip::Handle);
    CNetwork::AddListener(CPacketsID::ADD_MESSAGE_GXT, CPlayerPackets::AddMessageGXT::Handle);
    CNetwork::AddListener(CPacketsID::REMOVE_MESSAGE_GXT, CPlayerPackets::RemoveMessageGXT::Handle);
    CNetwork::AddListener(CPacketsID::CLEAR_ENTITY_BLIPS, CPlayerPackets::ClearEntityBlips::Handle);
    CNetwork::AddListener(CPacketsID::PLAY_MISSION_AUDIO, CPlayerPackets::PlayMissionAudio::Handle);
    CNetwork::AddListener(CPacketsID::UPDATE_CHECKPOINT, CPlayerPackets::UpdateCheckpoint::Handle);
    CNetwork::AddListener(CPacketsID::REMOVE_CHECKPOINT, CPlayerPackets::RemoveCheckpoint::Handle);
    CNetwork::AddListener(CPacketsID::ENEX_SYNC, CPlayerPackets::EnExSync::Handle);
    CNetwork::AddListener(CPacketsID::CREATE_STATIC_BLIP, CPlayerPackets::CreateStaticBlip::Handle);
    CNetwork::AddListener(CPacketsID::SET_VEHICLE_CREATED_BY, CVehiclePackets::SetVehicleCreatedBy::Handle);
    CNetwork::AddListener(CPacketsID::SET_PLAYER_TASK, CPlayerPackets::SetPlayerTask::Handle);
    CNetwork::AddListener(CPacketsID::PED_SAY, CPlayerPackets::PedSay::Handle);
    CNetwork::AddListener(CPacketsID::PED_CLAIM_ON_RELEASE, CPedPackets::PedClaimOnRelease::Handle);
    CNetwork::AddListener(CPacketsID::PED_CANCEL_CLAIM, CPedPackets::PedCancelClaim::Handle);
    CNetwork::AddListener(CPacketsID::PED_RESET_ALL_CLAIMS, CPedPackets::PedResetAllClaims::Handle);
    CNetwork::AddListener(CPacketsID::PED_TAKE_HOST, CPedPackets::PedTakeHost::Handle);
    CNetwork::AddListener(CPacketsID::PERFORM_TASK_SEQUENCE, CPedPackets::PerformTaskSequence::Handle);
    CNetwork::AddListener(CPacketsID::ADD_PROJECTILE, CPlayerPackets::AddProjectile::Handle);
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
    ENetPacket* packet = enet_packet_create(packetData, packetSize, flag);

    delete[] packetData;

    // send packet
    enet_peer_send(peer, 0, packet);
}

void CNetwork::SendPacketToAll(unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag, ENetPeer* dontShareWith)
{
    size_t packetSize = 2 + dataSize;
    char* packetData = new char[packetSize];
    memcpy(packetData, &id, 2);
    memcpy(packetData + 2, data, dataSize);
    ENetPacket* packet = enet_packet_create(packetData, packetSize, flag);

    delete[] packetData;

    for (int i = 0; i != CPlayerManager::m_pPlayers.size(); i++)
    {
        if (CPlayerManager::m_pPlayers[i]->m_pPeer != dontShareWith)
        {
            enet_peer_send(CPlayerManager::m_pPlayers[i]->m_pPeer, 0, packet);
        }
    }
}

void CNetwork::SendPacketRawToAll(void* data, size_t dataSize, ENetPacketFlag flag, ENetPeer* dontShareWith)
{
    ENetPacket* packet = enet_packet_create(data, dataSize, flag);

    for (int i = 0; i != CPlayerManager::m_pPlayers.size(); i++)
    {
        if (CPlayerManager::m_pPlayers[i]->m_pPeer != dontShareWith)
        {
            enet_peer_send(CPlayerManager::m_pPlayers[i]->m_pPeer, 0, packet);
        }
    }
}

void CNetwork::HandlePeerConnected(ENetEvent& event)
{
    printf("[Game] : A new client connected from %i.%i.%i.%i:%u.\n", 
        event.peer->address.host & 0xFF, 
        (event.peer->address.host >> 8) & 0xFF, 
        (event.peer->address.host >> 16) & 0xFF, 
        (event.peer->address.host >> 24) & 0xFF,
        event.peer->address.port);

    // set player disconnection timeout
    enet_peer_timeout(event.peer, 10000, 6000, 10000); //timeoutLimit, timeoutMinimum, timeoutMaximum
}

void CNetwork::HandlePlayerDisconnected(ENetEvent& event)
{
    // disconnect player

    // find player instance by enetpeer
    CPlayer* player = CPlayerManager::GetPlayer(event.peer);
    
    if (player == nullptr)
    {
        return;
    }

    CVehicle* vehicle = CVehicleManager::GetVehicle(player->m_nVehicleId);

    if (vehicle != nullptr)
    {
        vehicle->m_pPlayers[player->m_nSeatId] = nullptr;
    }
    
    if (CPlayerPackets::EnExSync::ms_pLastPlayerOwner == player)
    {
        CPlayerPackets::EnExSync::ms_pLastPlayerOwner = nullptr;
    }

    CPedManager::RemoveAllHostedAndNotify(player);
    CVehicleManager::RemoveAllHostedAndNotify(player);

    // remove
    CPlayerManager::Remove(player);

    // create PlayerDisconnected packet struct
    CPlayerPackets::PlayerDisconnected packet =
    {
        player->m_iPlayerId // id
    };

    // send to all
    CNetwork::SendPacketToAll(CPacketsID::PLAYER_DISCONNECTED, &packet, sizeof (CPlayerPackets::PlayerDisconnected), (ENetPacketFlag)0, event.peer);

    printf("[Game] : %i Disconnected.\n", player->m_iPlayerId);

    CPlayerManager::AssignHostToFirstPlayer();
}

void CNetwork::HandlePacketReceive(ENetEvent& event)
{
    // get packet id
    unsigned short id;
    memcpy(&id, event.packet->data, 2);

    if (id == CPacketsID::MASS_PACKET_SEQUENCE)
    {
        CNetwork::SendPacketRawToAll(event.packet->data, event.packet->dataLength, (ENetPacketFlag)event.packet->flags, event.peer);
    }
    else
    {
        // get data
        char* data = new char[event.packet->dataLength - 2];
        memcpy(data, event.packet->data + 2, event.packet->dataLength - 2);
        // call listener's callback by id
        auto it = m_packetListeners.find(id);
        if (it != m_packetListeners.end())
        {
            it->second->m_callback(event.peer, data, (int)event.packet->dataLength - 2);
        }

        delete[] data;
    }
}

void CNetwork::AddListener(unsigned short id, void(*callback)(ENetPeer*, void*, int))
{
    CPacketListener* listener = new CPacketListener(id, callback);
    m_packetListeners.insert({ id, listener });
}

void CNetwork::HandlePlayerConnected(ENetPeer* peer, void* data, int size)
{
    CPlayerPackets::PlayerConnected* packet = (CPlayerPackets::PlayerConnected*)data;

    int freeId = CPlayerManager::GetFreeID();
    CPlayer* player = new CPlayer(peer, freeId);
    strcpy_s(player->m_Name, packet->name);
    CPlayerManager::Add(player);

    uint32_t packedVersion = semver_parse(COOPANDREAS_VERSION, nullptr);
    char buffer[23];
    semver_t playerVersion;
    semver_unpack(packet->version, &playerVersion);
    semver_to_string(&playerVersion, buffer, sizeof(buffer));
    buffer[22] = '\0';

    if (packedVersion != packet->version)
    {
        CPlayerPackets::PlayerDisconnected packet{};
        packet.id = -1;
        packet.reason = 1;
        packet.version = packedVersion;
        CNetwork::SendPacket(peer, CPacketsID::PLAYER_DISCONNECTED, &packet, sizeof(packet), ENET_PACKET_FLAG_RELIABLE);

        enet_peer_disconnect_later(peer, packedVersion);
        return;
    }

    printf("freeId %d name %s version %s\n", freeId, packet->name, buffer);
    
    packet->id = freeId;

    CNetwork::SendPacketToAll(CPacketsID::PLAYER_CONNECTED, packet, sizeof(*packet), ENET_PACKET_FLAG_RELIABLE, peer);

    for (auto i : CPlayerManager::m_pPlayers)
    {
        if (i->m_iPlayerId == freeId)
            continue;

        CPlayerPackets::PlayerConnected newPlayerPacket{};
        newPlayerPacket.id = i->m_iPlayerId;
        newPlayerPacket.isAlreadyConnected = true;
        strcpy_s(newPlayerPacket.name, i->m_Name);

        CNetwork::SendPacket(peer, CPacketsID::PLAYER_CONNECTED, &newPlayerPacket, sizeof(CPlayerPackets::PlayerConnected), ENET_PACKET_FLAG_RELIABLE);

        if (i->m_ucSyncFlags.bStatsModified)
        {
            CPlayerPackets::PlayerStats statsPacket{};
            statsPacket.playerid = i->m_iPlayerId;
            memcpy(statsPacket.stats, i->m_afStats, sizeof(i->m_afStats));
            CNetwork::SendPacket(peer, CPacketsID::PLAYER_STATS, &statsPacket, sizeof(statsPacket), ENET_PACKET_FLAG_RELIABLE);
        }

        if (i->m_ucSyncFlags.bClothesModified)
        {
            CPlayerPackets::RebuildPlayer rebuildPacket{};
            rebuildPacket.playerid = i->m_iPlayerId;
            memcpy(rebuildPacket.m_anModelKeys, i->m_anModelKeys, sizeof(i->m_anModelKeys));
            memcpy(rebuildPacket.m_anTextureKeys, i->m_anTextureKeys, sizeof(i->m_anTextureKeys));
            rebuildPacket.m_fFatStat = i->m_fFatStat;
            rebuildPacket.m_fMuscleStat = i->m_fMuscleStat;
            CNetwork::SendPacket(peer, CPacketsID::REBUILD_PLAYER, &rebuildPacket, sizeof(rebuildPacket), ENET_PACKET_FLAG_RELIABLE);
        }

        if (i->m_ucSyncFlags.bWaypointModified)
        {
            CPlayerPackets::PlayerPlaceWaypoint waypointPacket{};
            waypointPacket.playerid = i->m_iPlayerId;
            waypointPacket.position = i->m_vecWaypointPos;
            waypointPacket.place = true;
            CNetwork::SendPacket(peer, CPacketsID::PLAYER_PLACE_WAYPOINT, &waypointPacket, sizeof(waypointPacket), ENET_PACKET_FLAG_RELIABLE);
        }
    }

    for (auto i : CVehicleManager::m_pVehicles)
    {
        CVehiclePackets::VehicleSpawn vehicleSpawnPacket{};
        vehicleSpawnPacket.vehicleid = i->m_nVehicleId;
        vehicleSpawnPacket.modelid = i->m_nModelId;
        vehicleSpawnPacket.pos = i->m_vecPosition;
        vehicleSpawnPacket.rot = static_cast<float>(i->m_vecRotation.z * (3.141592 / 180)); // convert to radians
        vehicleSpawnPacket.color1 = i->m_nPrimaryColor;
        vehicleSpawnPacket.color2 = i->m_nSecondaryColor;

        CNetwork::SendPacket(peer, CPacketsID::VEHICLE_SPAWN, &vehicleSpawnPacket, sizeof vehicleSpawnPacket, ENET_PACKET_FLAG_RELIABLE);

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
            CVehiclePackets::VehicleDamage vehicleDamagePacket{};
            vehicleDamagePacket.vehicleid = i->m_nVehicleId;
            memcpy(&vehicleDamagePacket.damageManager_padding, i->m_damageManager_padding, 23);
            CNetwork::SendPacket(peer, CPacketsID::VEHICLE_DAMAGE, &vehicleDamagePacket, sizeof vehicleDamagePacket, ENET_PACKET_FLAG_RELIABLE);
        }

        for (int component : i->m_pComponents)
        {
            CVehiclePackets::VehicleComponentAdd vehicleComponentAddPacket{};
            vehicleComponentAddPacket.vehicleid = i->m_nVehicleId;
            vehicleComponentAddPacket.componentid = component;
            CNetwork::SendPacket(peer, CPacketsID::VEHICLE_COMPONENT_ADD, &vehicleComponentAddPacket, sizeof vehicleComponentAddPacket, ENET_PACKET_FLAG_RELIABLE);
        }
    }

    for (auto i : CPedManager::m_pPeds)
    {
        CPedPackets::PedSpawn packet{};
        packet.pedid = i->m_nPedId;
        packet.modelId = i->m_nModelId;
        packet.pos = i->m_vecPos;
        packet.pedType = i->m_nPedType;
        packet.createdBy = i->m_nCreatedBy;
        strncpy_s(packet.specialModelName, i->m_szSpecialModelName, strlen(i->m_szSpecialModelName));
        CNetwork::SendPacket(peer, CPacketsID::PED_SPAWN, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    }

    if (CPlayerPackets::EnExSync::ms_pLastPlayerOwner)
    {
        if (std::find(CPlayerManager::m_pPlayers.begin(), CPlayerManager::m_pPlayers.end(), CPlayerPackets::EnExSync::ms_pLastPlayerOwner)
            != CPlayerManager::m_pPlayers.end())
        {
            CNetwork::SendPacket(peer, CPacketsID::ENEX_SYNC, CPlayerPackets::EnExSync::ms_vLastData.data(), CPlayerPackets::EnExSync::ms_vLastData.size(), ENET_PACKET_FLAG_RELIABLE);
        }
    }

    CPlayerPackets::PlayerHandshake handshakePacket = { freeId };
    CNetwork::SendPacket(peer, CPacketsID::PLAYER_HANDSHAKE, &handshakePacket, sizeof handshakePacket, ENET_PACKET_FLAG_RELIABLE);

    CPlayerManager::AssignHostToFirstPlayer();
}