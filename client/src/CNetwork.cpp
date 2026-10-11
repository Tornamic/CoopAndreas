#include "CPacketFactory.h"
#include "enet/enet.h"
#include "stdafx.h"
#include "../shared/semver.h"
#include <cassert>
#include <windows.h>

void CNetwork::Start()
{
    m_networkThread.ClearQueues();
    m_networkThread.Start(InitAsync);
}

DWORD WINAPI CNetwork::InitAsync(LPVOID)
{
    assert(strcmp(m_IpAddress, "") != 0 && "Wrong IP passed");
    assert(m_nPort != 0 && "Wrong Port passed");

    if (enet_initialize() != 0)
    {
        CChat::AddMessage("{63c7e8}[Network] {f07878}Failed to enet_initialize.");
        return 0;
    }
    else
    {
        std::cout << "Success to enet_initialize" << std::endl;
    }

    m_pENetHost = enet_host_create(NULL, 1, (int)ePacketChannel::COUNT, 0, 0);
    if (m_pENetHost == nullptr)
    {
        enet_deinitialize();
        return 0;
    }

    ENetAddress address;

    enet_address_set_host(&address, m_IpAddress);
    address.port = m_nPort;

    uint32_t packedVersion = semver_parse(COOPANDREAS_VERSION, nullptr);  // v0.1.0-alpha compat
    m_pPeer = enet_host_connect(m_pENetHost, &address, (int)ePacketChannel::COUNT, packedVersion);
    if (m_pPeer == nullptr)
    {
        CChat::AddMessage("{63c7e8}[Network] {ff0000}m_pPeer == NULL.");
        enet_host_destroy(m_pENetHost);
        m_pENetHost = nullptr;
        enet_deinitialize();
        return 0;
    }

    CChat::AddMessage("{63c7e8}[Network] {d6dce8}Connecting to the server...");
    ENetEvent event;
    while (!m_bConnected && !m_networkThread.IsStopRequested())
    {
        if (enet_host_service(m_pENetHost, &event, 100) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
        {
            m_bConnected = true;
            m_bAuthenticated = false;
            m_bHandshakePending = true;

            CChat::AddMessage("{63c7e8}[Network] {78d99a}Successfully {cecedb}connected to the server.");
        }
        else if (!m_networkThread.IsStopRequested())
        {
            CChat::AddMessage("{63c7e8}[Network] {e8b86d}Failed to connect. Retrying...");
        }
    }

    while (!m_networkThread.IsStopRequested())
    {
        std::deque<CNetworkThread::QueuedPacket> packetsToSend = m_networkThread.TakeOutgoing();

        for (const CNetworkThread::QueuedPacket& queuedPacket : packetsToSend)
        {
            uint32_t flags = queuedPacket.reliability == ePacketReliability::RELIABLE ? ENET_PACKET_FLAG_RELIABLE : 0;
            ENetPacket* packet = enet_packet_create(queuedPacket.data.data(), queuedPacket.data.size(), flags);
            if (packet != nullptr)
            {
                enet_peer_send(m_pPeer, static_cast<uint8_t>(queuedPacket.channel), packet);
                ms_nBytesSentThisSecondCounter += static_cast<uint32_t>(queuedPacket.data.size());
                ++m_nSentPackets;
            }
        }

        while (enet_host_service(m_pENetHost, &event, 1) > 0)
        {
            switch (event.type)
            {
                case ENET_EVENT_TYPE_RECEIVE:
                {
                    ms_nBytesReceivedThisSecondCounter += event.packet->dataLength;
                    ++m_nReceivedPackets;
                    std::vector<uint8_t> data(event.packet->data, event.packet->data + event.packet->dataLength);
                    m_networkThread.EnqueueIncoming(std::move(data));
                    enet_packet_destroy(event.packet);
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT:
                    m_bConnected = false;
                    m_bAuthenticated = false;
                    m_networkThread.RequestStop();
                    break;
                default:
                    break;
            }
        }

        if (m_pPeer != nullptr)
        {
            m_nRTT = m_pPeer->roundTripTime;
            m_nRTTVariance = m_pPeer->roundTripTimeVariance;
            m_nDroppedPackets = m_pPeer->packetsLost;
            m_nPacketLoss = m_pPeer->packetLoss;
        }
    }

    if (m_pPeer != nullptr)
    {
        enet_peer_disconnect_now(m_pPeer, 0);
    }
    if (m_pENetHost != nullptr)
    {
        enet_host_destroy(m_pENetHost);
    }
    m_pPeer = nullptr;
    m_pENetHost = nullptr;
    enet_deinitialize();
    m_bConnected = false;
    m_bAuthenticated = false;
    m_bHandshakePending = false;
    return true;
}

void CNetwork::ProcessReceive()
{
    if (m_bHandshakePending.exchange(false))
    {
        Packets::System::PlayerConnected connectedPacket{};
        connectedPacket.payload.isAlreadyConnected = false;
        connectedPacket.payload.playerid = -1;
        connectedPacket.payload.version = semver_parse(COOPANDREAS_VERSION, nullptr);
        strcpy_s(connectedPacket.payload.name, CLocalPlayer::m_Name);
        GetPacketFactory().Send(connectedPacket);
    }

    std::deque<std::vector<uint8_t>> packets = m_networkThread.TakeIncoming();

    for (const std::vector<uint8_t>& packet : packets)
        GetPacketFactory().Receive(packet.data(), static_cast<int>(packet.size()));
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

    if (data == nullptr || dataSize <= 0)
        return;

    CNetworkThread::QueuedPacket queuedPacket;
    queuedPacket.data.assign(data, data + dataSize);
    queuedPacket.channel = packetChannel;
    queuedPacket.reliability = packetReliability;
    m_networkThread.EnqueueOutgoing(std::move(queuedPacket));
}

void CNetwork::Disconnect()
{
    m_bConnected = false;
    m_bAuthenticated = false;
    m_networkThread.Stop();
}
