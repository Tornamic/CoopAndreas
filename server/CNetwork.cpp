#include "stdafx.h"

bool CNetwork::Init(unsigned short port)
{

    if (enet_initialize() != 0) // try to init enet
    {
        printf("init failed");
        return false;
    }

    ENetAddress address;

    address.host = ENET_HOST_ANY; // bind server ip
    address.port = port; // bind server port

    ENetHost* server = enet_host_create(&address, MAX_SERVER_PLAYERS, 2, 0, 0); // create enet host

    if (server == NULL)
    {
        printf("creating failed");
        return false;
    }

    printf("Server stared on port %d\n", port);

    ENetEvent event;
    while (true) // waiting for event
    {
        enet_host_service(server, &event, 10);
        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
            {
                printf("A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);

                enet_peer_timeout(event.peer, 1000, 300, 500); //timeoutLimit, timeoutMinimum, timeoutMaximum
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
                }

                break;
            }

            case ENET_EVENT_TYPE_RECEIVE:
            {
                unsigned short id;

                // extract id
                memcpy(&id, event.packet->data, 2);

                // extract size
                size_t dataSize = event.packet->dataLength - 2;

                // allocate memory for data
                char* data = new char[dataSize];

                // extract data
                memcpy(data, event.packet->data + 2, dataSize);

                CPackets::PlayerOnFoot* packet = (CPackets::PlayerOnFoot*)data;

                packet->id = CPlayerManager::GetPlayer(event.peer)->m_iPlayerId;
                CNetwork::SendPacketToAll(CPacketsID::PLAYER_ONFOOT, packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE, event.peer);

                enet_packet_destroy(event.packet);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                printf("%s disconnected.\n", event.peer->data);

                // disconnect player

                // find player instance by enetpeer
                CPlayer* player = CPlayerManager::GetPlayer(event.peer);

                // remove
                CPlayerManager::Remove(player);

                // create PlayerDisconnected packet struct
                CPackets::PlayerDisconnected packet =
                {
                    player->m_iPlayerId // id
                };

                // send to all
                CNetwork::SendPacketToAll(CPacketsID::PLAYER_DISCONNECTED, &packet, sizeof CPackets::PlayerDisconnected, ENET_PACKET_FLAG_RELIABLE, event.peer);

                event.peer->data = NULL;
                break;
            }
            case ENET_EVENT_TYPE_NONE:
                break;
        }
    }

    enet_host_destroy(server);
    enet_deinitialize();
    std::cout << "Destroyed" << std::endl;
    return 0;
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

    // send packet
    enet_peer_send(peer, 0, packet);
}

void CNetwork::SendPacketToAll(unsigned short id, void* data, size_t dataSize, ENetPacketFlag flag, ENetPeer* dontShareWith = nullptr)
{
    size_t packetSize = 2 + dataSize;
    char* packetData = new char[packetSize];
    memcpy(packetData, &id, 2);
    memcpy(packetData + 2, data, dataSize);
    ENetPacket* packet = enet_packet_create(packetData, packetSize, flag);

    for (int i = 0; i != CPlayerManager::m_pPlayers.size(); i++)
    {
        if (CPlayerManager::m_pPlayers[i]->m_pPeer != dontShareWith)
        {
            enet_peer_send(CPlayerManager::m_pPlayers[i]->m_pPeer, 0, packet);
        }
    }
}
void CNetwork::SendPacketRawToAll(void* data, size_t dataSize, ENetPacketFlag flag, ENetPeer* dontShareWith = nullptr)
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