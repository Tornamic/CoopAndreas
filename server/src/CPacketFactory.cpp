#include "network/packet.h"
#include "network/packet_handler.h"
#include "network/packet_types.h"
#include "network/packets/system.h"
#include "serialize.h"
#include "stdafx.h"
#include "CPacketFactory.h"

static uint8_t packetBuffer[10 * 1024];

static void BuildPacketStream(Packet& packet, serialize::WriteStream& writeStream, ePacketChannel& outPacketChannel,
    ePacketReliability& outPacketReliability)
{
    memset(packetBuffer, 0, sizeof(packetBuffer));  // is this necessary?

    writeStream = serialize::WriteStream(packetBuffer, sizeof(packetBuffer));

    uint16_t packetType = static_cast<uint16_t>(packet.GetType());
    writeStream.SerializeBits(packetType, 16);

    if (packet.GetChannel() != ePacketChannel::SYSTEM)
    {
        if (packet.serverTime == 0)
        {
            packet.serverTime = g_serverTime;
        }
        writeStream.SerializeBits(packet.serverTime, 32);
    }

    if (!packet.SerializeWrite(writeStream))
    {
        logger::warn("SerializeWrite (packet type #%d) returned false", packetType);
        return;
    }
    writeStream.Flush();

    outPacketChannel = packet.GetChannel();
    outPacketReliability = GetChannelReliability(outPacketChannel);
}

void CPacketFactory::Send(Packet& packet, CNetworkPlayer* pNetworkPlayer)
{
    serialize::WriteStream writeStream;
    ePacketChannel packetChannel{};
    ePacketReliability packetReliability{};
    BuildPacketStream(packet, writeStream, packetChannel, packetReliability);

    CNetwork::SendPacket(
        pNetworkPlayer, writeStream.GetData(), writeStream.GetBytesProcessed(), packetChannel, packetReliability);
}

void CPacketFactory::SendPacketNoAuth_ENet(Packet& packet, ENetPeer* pENetPeer)
{
    serialize::WriteStream writeStream;
    ePacketChannel packetChannel{};
    ePacketReliability packetReliability{};
    BuildPacketStream(packet, writeStream, packetChannel, packetReliability);

    CNetwork::SendPacketNoAuth_ENet(
        pENetPeer, writeStream.GetData(), writeStream.GetBytesProcessed(), packetChannel, packetReliability);
}

void CPacketFactory::SendToAll(Packet& packet, CNetworkPlayer* pNetworkPlayerToIgnore)
{
    serialize::WriteStream writeStream;
    ePacketChannel packetChannel{};
    ePacketReliability packetReliability{};
    BuildPacketStream(packet, writeStream, packetChannel, packetReliability);

    CNetwork::SendPacketToAll(writeStream.GetData(), writeStream.GetBytesProcessed(), packetChannel, packetReliability,
        pNetworkPlayerToIgnore);
}

void CPacketFactory::ReceivePacketNoAuth_ENet(const uint8_t* data, int dataSize, ENetPeer* pENetPeer)
{
    serialize::ReadStream readStream = serialize::ReadStream(data, dataSize);

    uint32_t tempPacketType = 0;
    if (!readStream.SerializeBits(tempPacketType, 16))
    {
        logger::warn("SerializeBits couldn't read tempPacketType");
        return;
    }

    ePacketType packetType = static_cast<ePacketType>(tempPacketType);
    if (packetType != ePacketType::PLAYER_CONNECTED)
    {
        logger::warn("An unauthorized player is trying to send a packet type #%d %i.%i.%i.%i:%u", tempPacketType,
            pENetPeer->address.host & 0xFF, (pENetPeer->address.host >> 8) & 0xFF,
            (pENetPeer->address.host >> 16) & 0xFF, (pENetPeer->address.host >> 24) & 0xFF, pENetPeer->address.port);
        return;
    }

    Packets::System::PlayerConnected playerConnected{};
    if (!playerConnected.SerializeRead(readStream))
    {
        logger::warn("Packets::System::PlayerConnected::SerializeRead returned false");
        return;
    }
    CNetwork::HandlePlayerConnected(pENetPeer, playerConnected);
}

void CPacketFactory::Receive(const uint8_t* data, int dataSize, CNetworkPlayer* pNetworkPlayer)
{
    serialize::ReadStream readStream = serialize::ReadStream(data, dataSize);

    uint32_t tempPacketType = 0;
    if (!readStream.SerializeBits(tempPacketType, 16))
    {
        logger::warn("SerializeBits couldn't read tempPacketType");
        return;
    }
    ePacketType packetType = static_cast<ePacketType>(tempPacketType);

    Packet* pPacket = m_map[packetType];
    if (pPacket == nullptr)
    {
        logger::warn("Couldn't find packet instance of #%d", packetType);
        return;
    }

    if (pPacket->GetChannel() != ePacketChannel::SYSTEM)
    {
        if (!readStream.SerializeBits(pPacket->serverTime, 32))
        {
            logger::warn("SerializeBits couldn't read serverTime");
            return;
        }
    }

    if (!pPacket->SerializeRead(readStream))
    {
        logger::warn("SerializeRead (packet type #%d) returned false", packetType);
        return;
    }

    GetPacketHandler().ProcessPacket(pPacket, pNetworkPlayer);
}

void CPacketFactory::RegisterPacket(Packet* pPacket)
{
    assert(pPacket != nullptr);

    ePacketType packetType = pPacket->GetType();

    assert(m_map.find(packetType) == m_map.end());

    m_map[packetType] = pPacket;
}