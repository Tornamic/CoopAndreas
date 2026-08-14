#include "CPacketFactory.h"
#include "CWeatherSync.h"
#include "enet/enet.h"
#include "stdafx.h"
#include "../shared/semver.h"
#include <cassert>
#include <windows.h>

DWORD WINAPI CNetwork::InitAsync(LPVOID)
{
    assert(strcmp(m_IpAddress, "") != 0 && "Wrong IP passed");
    assert(m_nPort != 0 && "Wrong Port passed");

    CWeatherSync::Reset();

    if (enet_initialize() != 0)
    {
        CChat::AddMessage("{cecedb}[Network] {ff0000}Failed to enet_initialize.");
        return false;
    }
    else
    {
        std::cout << "Success to enet_initialize" << std::endl;
    }

    m_pENetHost = enet_host_create(NULL, 1, (int)ePacketChannel::COUNT, 0, 0);

    if (m_pENetHost == NULL)
        return false;

    ENetAddress address;

    enet_address_set_host(&address, m_IpAddress);

    address.port = m_nPort;

    uint32_t packedVersion = semver_parse(COOPANDREAS_VERSION, nullptr);

    m_pPeer = enet_host_connect(m_pENetHost, &address, (int)ePacketChannel::COUNT, packedVersion);

    if (m_pPeer == NULL)
    {
        CChat::AddMessage("{cecedb}[Network] {ff0000}m_pPeer == NULL.");

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

            CWeatherSync::Reset();

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
            CChat::AddMessage("{cecedb}[Network] Failed to connect. Retrying...");
        }
    }

    return true;
}

void CNetwork::ProcessReceive()
{
#if true
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
                CNetwork::Disconnect();
                break;
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

#if true
    uint32_t eNetPacketFlags = 0;

    if (packetReliability == ePacketReliability::RELIABLE)
    {
        eNetPacketFlags |= ENET_PACKET_FLAG_RELIABLE;
    }
    else if (packetReliability == ePacketReliability::UNRELIABLE) {}

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

    m_bConnected = false;
    m_bAuthenticated = false;

    CWeatherSync::Reset();
}