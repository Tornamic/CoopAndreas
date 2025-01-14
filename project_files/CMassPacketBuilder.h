#pragma once
#include "stdafx.h"

class CMassPacketBuilder
{
public:
    CMassPacketBuilder()
    {
    }

    ~CMassPacketBuilder()
    {
        ClearPackets();
    }

    void AddPacket(unsigned short id, const void* data, size_t dataSize)
    {
        PacketInfo packetInfo;
        packetInfo.id = id;
        packetInfo.dataSize = dataSize;
        packetInfo.data = new char[dataSize];
        memcpy(packetInfo.data, data, dataSize);

        m_packets.push_back(packetInfo);
    }

    void Send(ENetPacketFlag flags = (ENetPacketFlag)0)
    {
        if (m_packets.empty())
            return;

        if (m_packets.size() == 1)
        {
            const auto& packet = m_packets[0];
            CNetwork::SendPacket(packet.id, packet.data, packet.dataSize, flags);
            ClearPackets();
            return;
        }

        size_t totalSize = 1;
        for (const auto& packet : m_packets)
        {
            totalSize += 2 + packet.dataSize;
        }

        char* buffer = new char[totalSize];

        buffer[0] = static_cast<unsigned char>(m_packets.size());

        size_t offset = 1;
        for (const auto& packet : m_packets)
        {
            memcpy(buffer + offset, &packet.id, 2);
            offset += 2;

            memcpy(buffer + offset, packet.data, packet.dataSize);
            offset += packet.dataSize;
        }

        CNetwork::SendPacket(CPacketsID::MASS_PACKET_SEQUENCE, buffer, totalSize, flags);

        delete[] buffer;
        ClearPackets();
    }

private:
    struct PacketInfo
    {
        unsigned short id;
        size_t dataSize;
        char* data;
    };

    std::vector<PacketInfo> m_packets;

    void ClearPackets()
    {
        for (auto& packet : m_packets)
        {
            delete[] packet.data;
        }
        m_packets.clear();
    }
};
