#include "CPacketFactory.h"
#include "enet/enet.h"
#include "stdafx.h"
#include "../shared/semver.h"
#include <cassert>
#include <windows.h>

DWORD WINAPI CNetwork::InitAsync(LPVOID)
{
    assert(strcmp(m_IpAddress, "") != 0 && "Wrong IP passed");
    assert(m_nPort != 0 && "Wrong Port passed");

    if (enet_initialize() != 0)
    {  // try to init enet
        CChat::AddMessage("{cecedb}[Network] {ff0000}Failed to enet_initialize.");
        return false;
    }
    else
    {
        std::cout << "Success to enet_initialize" << std::endl;
    }

    m_pENetHost = enet_host_create(NULL, 1, (int)ePacketChannel::COUNT, 0, 0);  // create enet client
    if (m_pENetHost == NULL)                           // check client
        return false;

    ENetAddress address;                           // connection address

    enet_address_set_host(&address, m_IpAddress);  // set address ip
    address.port = m_nPort;                        // set address port

    uint32_t packedVersion = semver_parse(COOPANDREAS_VERSION, nullptr); // v0.1.0-alpha compat
    m_pPeer =
        enet_host_connect(m_pENetHost, &address, (int)ePacketChannel::COUNT, packedVersion);  // connect to the server
    if (m_pPeer == NULL)
    {                                                                                         // if not connected
        CChat::AddMessage("{cecedb}[Network] {ff0000}m_pPeer == NULL.");
        // std::cout << "Not Connected" << std::endl;
        return false;
    }

    CChat::AddMessage("{cecedb}[Network] Connecting to the server...");
    ENetEvent event;
    while (!m_bConnected)
    {
        if (enet_host_service(m_pENetHost, &event, 2000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
        {
            m_bConnected = true;
            m_bAuthenticated = false;

            CChat::AddMessage("{cecedb}[Network] {00ff00}Successfully {cecedb}connected to the server.");

            Packets::System::PlayerConnected connectedPacket{};
            connectedPacket.payload.isAlreadyConnected = false;
            connectedPacket.payload.playerid = -1;
            connectedPacket.payload.version = packedVersion;
            strcpy_s(connectedPacket.payload.name, CLocalPlayer::m_Name);

            GetPacketFactory().Send(connectedPacket);
        }
        else
        {
            // enet_peer_reset(m_pPeer);
            CChat::AddMessage("{cecedb}[Network] Failed to connect. Retrying...");
        }
    }

    return true;
}

void CNetwork::ProcessReceive()
{
#if true  // ENET LAYER
    ENetEvent eNetEvent{};
    while (enet_host_service(m_pENetHost, &eNetEvent, 0) > 0)
    {
        switch (eNetEvent.type)
        {
            case ENET_EVENT_TYPE_RECEIVE:
            {
                ms_nBytesReceivedThisSecondCounter += eNetEvent.packet->dataLength;
                
                GetPacketFactory().Receive(eNetEvent.packet->data, eNetEvent.packet->dataLength);

                enet_packet_destroy(eNetEvent.packet);
                break;
            }
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                //assert(false && "TODO");
                CNetwork::Disconnect();
            }
            case ENET_EVENT_TYPE_NONE:
            case ENET_EVENT_TYPE_CONNECT:
                break;
        }
    }
#endif
}

void CNetwork::SendPacket(
    const uint8_t* data, int dataSize, ePacketChannel packetChannel, ePacketReliability packetReliability)
{
    if (!CNetwork::m_bConnected)
    {
#if DEBUG
        if (data != nullptr)
        {
            std::cerr << "PacketType #" << std::to_string(*reinterpret_cast<const uint16_t*>(data))
                      << " was sent while not connected";
        }
#endif
        return;
    }

#if true  // ENET LAYER
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
        enet_peer_send(m_pPeer, static_cast<uint8_t>(packetChannel), pENetPacket);
        ms_nBytesSentThisSecondCounter += dataSize;
    }
#endif
}

void CNetwork::Disconnect()
{
    if (!m_bConnected)
        return;

    enet_peer_disconnect_now(CNetwork::m_pPeer, 0);
    // enet_host_flush(CNetwork::m_pENetHost);
    // enet_peer_reset(CNetwork::m_pPeer);

    m_bConnected = false;
    m_bAuthenticated = false;
}