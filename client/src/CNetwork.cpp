#include "../shared/semver.h"
#include "stdafx.h"
#include "yojimbo.h"
#include "yojimbo_address.h"
#include "yojimbo_allocator.h"
#include "yojimbo_client.h"
#include "yojimbo_config.h"


ENetHost* CNetwork::m_pClient = nullptr;
ENetPeer* CNetwork::m_pPeer = nullptr;
bool CNetwork::m_bConnected = false;

std::unordered_map<unsigned short, CPacketListener*> CNetwork::m_packetListeners;

char CNetwork::m_IpAddress[128 + 1];
unsigned short CNetwork::m_nPort = Config::DEFAULT_PORT;

struct TestPlayerOnFoot : public yojimbo::Message
{
    int id = 0;
    CVector position = CVector();
    CVector velocity = CVector();
    float currentRotation = 0.0f;
    float aimingRotation = 0.0f;
    unsigned char health = 100;
    unsigned char armour = 0;
    unsigned char weapon = 0;
    unsigned char weaponState = 0;
    unsigned short ammo = 0;
    bool ducking = false;
    bool hasJetpack = false;
    char fightingStyle = 4;

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, id, 0, MAX_SERVER_PLAYERS);
        serialize_float(stream, position.x);
        serialize_float(stream, position.z);
        serialize_float(stream, position.y);
        printf("id %d pos {%f %f %f}\n", id, position.x, position.y, position.z);
        return true;
    }

    bool SerializeInternal(class yojimbo ::ReadStream& stream) { return Serialize(stream); };
    bool SerializeInternal(class yojimbo ::WriteStream& stream) { return Serialize(stream); };
    bool SerializeInternal(class yojimbo ::MeasureStream& stream) { return Serialize(stream); };
};

YOJIMBO_MESSAGE_FACTORY_START(TestMessageFactory, CPacketsID::PACKET_ID_MAX);
YOJIMBO_DECLARE_MESSAGE_TYPE(CPacketsID::PLAYER_ONFOOT, TestPlayerOnFoot);
YOJIMBO_MESSAGE_FACTORY_FINISH();

class ServerAdapter : public yojimbo::Adapter
{
public:
    void OnServerClientConnected(int clientIndex) override
    {
        printf("OnServerClientConnected %d", clientIndex);
        //CNetwork::HandlePeerConnected
        return;
    }

    yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator)
    {
        return YOJIMBO_NEW(allocator, TestMessageFactory, allocator);
    }
};

static ServerAdapter adapter;

DWORD WINAPI CNetwork::InitAsync(LPVOID)
{
    assert(strcmp(m_IpAddress, "") != 0 && "Wrong IP passed");
    assert(m_nPort != 0 && "Wrong Port passed");

    InitializeYojimbo();  // TODO: ShutdownYojimbo();

    double time = 100.0;

    uint64_t clientId = 0;
    yojimbo_random_bytes((uint8_t*)&clientId, 8);
    printf("client id is %.16" PRIx64 "\n", clientId);

    yojimbo::ClientServerConfig config;

    yojimbo::Client client(yojimbo::GetDefaultAllocator(), yojimbo::Address("0.0.0.0"), config, adapter, time);

    yojimbo::Address serverAddress(m_IpAddress, m_nPort);

    // init listeners
    CNetwork::InitListeners();

    if (enet_initialize() != 0)
    {  // try to init enet
        CChat::AddMessage("{cecedb}[Network] {ff0000}Failed to enet_initialize.");
        return false;
    }
    else
    {
        std::cout << "Success to enet_initialize" << std::endl;
    }

    m_pClient = enet_host_create(NULL, 1, 2, 0, 0);  // create enet client
    if (m_pClient == NULL)                           // check client
        return false;

    ENetAddress address;                           // connection address

    enet_address_set_host(&address, m_IpAddress);  // set address ip
    address.port = m_nPort;                        // set address port

    uint32_t packedVersion = semver_parse(COOPANDREAS_VERSION, nullptr);
    m_pPeer = enet_host_connect(m_pClient, &address, 2, packedVersion);  // connect to the server
    if (m_pPeer == NULL)
    {                                                                    // if not connected
        CChat::AddMessage("{cecedb}[Network] {ff0000}m_pPeer == NULL.");
        // std::cout << "Not Connected" << std::endl;
        return false;
    }

    CChat::AddMessage("{cecedb}[Network] Connecting to the server...");
    ENetEvent event;
    while (!m_bConnected)
    {
        if (enet_host_service(m_pClient, &event, 2000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
        {
            m_bConnected = true;
            CChat::AddMessage("{cecedb}[Network] {00ff00}Successfully {cecedb}connected to the server.");
            CPatch::RevertTemporaryPatches();

            CPackets::PlayerConnected packet{};
            strcpy_s(packet.name, CLocalPlayer::m_Name);
            packet.version = packedVersion;
            CNetwork::SendPacket(CPacketsID::PLAYER_CONNECTED, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
        }
        else
        {
            // enet_peer_reset(m_pPeer);
            CChat::AddMessage("{cecedb}[Network] Failed to connect. Retrying...");
        }
    }

    return true;
}

void CNetwork::SendPacket(unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag)
{
    if (!CNetwork::m_bConnected)
    {
        static int notSentPacketCount;
        std::cout << "Trying to send a packet " << std::to_string(id) << " while not connected "
                  << std::to_string(notSentPacketCount++) << std::endl;
        return;
    }

    // packet size `id + data`
    size_t packetSize = 2 + dataSize;

    // create buffer using vector
    std::vector<char> packetData(packetSize);

    // copy id
    memcpy(packetData.data(), &id, 2);

    // copy data
    memcpy(packetData.data() + 2, data, dataSize);

    // create packet
    ENetPacket* packet = enet_packet_create(packetData.data(), packetSize, flag);

    // send packet
    enet_peer_send(m_pPeer, 0, packet);

    ms_nBytesSentThisSecondCounter += dataSize;
}

void CNetwork::Disconnect()
{
    /*enet_peer_disconnect_now(CNetwork::m_pPeer, 0);
    enet_host_flush(CNetwork::m_pClient);
    enet_peer_reset(CNetwork::m_pPeer);*/

    m_bConnected = false;
}

void CNetwork::InitListeners()
{
    CNetwork::AddListener(CPacketsID::PLAYER_ONFOOT, CPacketHandler::PlayerOnFoot__Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_CONNECTED, CPacketHandler::PlayerConnected__Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_DISCONNECTED, CPacketHandler::PlayerDisconnected__Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_BULLET_SHOT, CPacketHandler::PlayerBulletShot__Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_HANDSHAKE, CPacketHandler::PlayerHandshake__Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_PLACE_WAYPOINT, CPacketHandler::PlayerPlaceWaypoint__Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_SET_HOST, CPacketHandler::PlayerSetHost__Handle);
    CNetwork::AddListener(CPacketsID::ADD_EXPLOSION, CPacketHandler::AddExplosion__Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_SPAWN, CPacketHandler::VehicleSpawn__Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_REMOVE, CPacketHandler::VehicleRemove__Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_ENTER, CPacketHandler::VehicleEnter__Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_EXIT, CPacketHandler::VehicleExit__Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_DRIVER_UPDATE, CPacketHandler::VehicleDriverUpdate__Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_IDLE_UPDATE, CPacketHandler::VehicleIdleUpdate__Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_DAMAGE, CPacketHandler::VehicleDamage__Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_COMPONENT_ADD, CPacketHandler::VehicleComponentAdd__Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_COMPONENT_REMOVE, CPacketHandler::VehicleComponentRemove__Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_PASSENGER_UPDATE, CPacketHandler::VehiclePassengerUpdate__Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_CHAT_MESSAGE, CPacketHandler::PlayerChatMessage__Handle);
    CNetwork::AddListener(CPacketsID::PED_SPAWN, CPacketHandler::PedSpawn__Handle);
    CNetwork::AddListener(CPacketsID::PED_REMOVE, CPacketHandler::PedRemove__Handle);
    CNetwork::AddListener(CPacketsID::PED_ONFOOT, CPacketHandler::PedOnFoot__Handle);
    CNetwork::AddListener(CPacketsID::GAME_WEATHER_TIME, CPacketHandler::GameWeatherTime__Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_KEY_SYNC, CPacketHandler::PlayerKeySync__Handle);
    CNetwork::AddListener(CPacketsID::PED_ADD_TASK, CPacketHandler::PedAddTask__Handle);
    CNetwork::AddListener(CPacketsID::PED_DRIVER_UPDATE, CPacketHandler::PedDriverUpdate__Handle);
    CNetwork::AddListener(CPacketsID::PED_SHOT_SYNC, CPacketHandler::PedShotSync__Handle);
    CNetwork::AddListener(CPacketsID::PED_PASSENGER_UPDATE, CPacketHandler::PedPassengerSync__Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_AIM_SYNC, CPacketHandler::PlayerAimSync__Handle);
    CNetwork::AddListener(CPacketsID::VEHICLE_CONFIRM, CPacketHandler::VehicleConfirm__Handle);
    CNetwork::AddListener(CPacketsID::PED_CONFIRM, CPacketHandler::PedConfirm__Handle);
    CNetwork::AddListener(CPacketsID::PLAYER_STATS, CPacketHandler::PlayerStats__Handle);
    CNetwork::AddListener(CPacketsID::REBUILD_PLAYER, CPacketHandler::RebuildPlayer__Handle);
    CNetwork::AddListener(CPacketsID::RESPAWN_PLAYER, CPacketHandler::RespawnPlayer__Handle);
    CNetwork::AddListener(CPacketsID::ASSIGN_VEHICLE, CPacketHandler::AssignVehicleSyncer__Handle);
    CNetwork::AddListener(CPacketsID::ASSIGN_PED, CPacketHandler::AssignPedSyncer__Handle);
    CNetwork::AddListener(CPacketsID::MASS_PACKET_SEQUENCE, CPacketHandler::MassPacketSequence__Handle);
    // CNetwork::AddListener(CPacketsID::START_CUTSCENE, CPacketHandler::StartCutscene__Handle);
    // CNetwork::AddListener(CPacketsID::SKIP_CUTSCENE, CPacketHandler::SkipCutscene__Handle);
    CNetwork::AddListener(CPacketsID::OPCODE_SYNC, CPacketHandler::OpCodeSync__Handle);
    CNetwork::AddListener(CPacketsID::ON_MISSION_FLAG_SYNC, CPacketHandler::OnMissionFlagSync__Handle);
    CNetwork::AddListener(CPacketsID::UPDATE_ENTITY_BLIP, CPacketHandler::UpdateEntityBlip__Handle);
    CNetwork::AddListener(CPacketsID::REMOVE_ENTITY_BLIP, CPacketHandler::RemoveEntityBlip__Handle);
    CNetwork::AddListener(CPacketsID::ADD_MESSAGE_GXT, CPacketHandler::AddMessageGXT__Handle);
    CNetwork::AddListener(CPacketsID::REMOVE_MESSAGE_GXT, CPacketHandler::RemoveMessageGXT__Handle);
    CNetwork::AddListener(CPacketsID::CLEAR_ENTITY_BLIPS, CPacketHandler::ClearEntityBlips__Handle);
    CNetwork::AddListener(CPacketsID::PLAY_MISSION_AUDIO, CPacketHandler::PlayMissionAudio__Handle);
    CNetwork::AddListener(CPacketsID::UPDATE_CHECKPOINT, CPacketHandler::UpdateCheckpoint__Handle);
    CNetwork::AddListener(CPacketsID::REMOVE_CHECKPOINT, CPacketHandler::RemoveCheckpoint__Handle);
    CNetwork::AddListener(CPacketsID::ENEX_SYNC, CPacketHandler::EnExSync__Handle);
    CNetwork::AddListener(CPacketsID::CREATE_STATIC_BLIP, CPacketHandler::CreateMissionMarker__Handle);
    CNetwork::AddListener(CPacketsID::SET_VEHICLE_CREATED_BY, CPacketHandler::SetVehicleCreatedBy__Handle);
    CNetwork::AddListener(CPacketsID::SET_PLAYER_TASK, CPacketHandler::SetPlayerTask__Handle);
    CNetwork::AddListener(CPacketsID::PED_SAY, CPacketHandler::PedSay__Handle);
    CNetwork::AddListener(CPacketsID::PED_RESET_ALL_CLAIMS, CPacketHandler::PedResetAllClaims__Handle);
    CNetwork::AddListener(CPacketsID::PERFORM_TASK_SEQUENCE, CPacketHandler::PerformTaskSequence__Handle);
    CNetwork::AddListener(CPacketsID::ADD_PROJECTILE, CPacketHandler::AddProjectile__Handle);
    CNetwork::AddListener(CPacketsID::TAG_UPDATE, CPacketHandler::TagUpdate__Handle);
    CNetwork::AddListener(CPacketsID::UPDATE_ALL_TAGS, CPacketHandler::UpdateAllTags__Handle);
    CNetwork::AddListener(CPacketsID::TELEPORT_PLAYER_SCRIPTED, CPacketHandler::TeleportPlayerScripted__Handle);
}

void CNetwork::HandlePacketReceive(ENetEvent& event)
{
    uint16_t packetid;
    memcpy(&packetid, event.packet->data, sizeof(uint16_t));

    auto it = m_packetListeners.find(packetid);
    if (it != m_packetListeners.end())
    {
        uint8_t* pData = event.packet->data + sizeof(uint16_t);
        int32_t nLength = event.packet->dataLength - sizeof(uint16_t);
        it->second->m_callback(pData, nLength);
    }
}

void CNetwork::AddListener(unsigned short id, void (*callback)(void*, int))
{
    CPacketListener* listener = new CPacketListener(id, callback);
    m_packetListeners.insert({id, listener});
}