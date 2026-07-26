#include "CPacketBuffer.h"
#include "CServerTime.h"
#include "network/packet.h"
#include "stdafx.h"

void CPacketFactory::Send(Packet& packet)
{
    static uint8_t packetBuffer[10 * 1024];
    memset(packetBuffer, 0, sizeof(packetBuffer));  // is this necessary?

    serialize::WriteStream writeStream = serialize::WriteStream(packetBuffer, sizeof(packetBuffer));

    uint16_t packetType = static_cast<uint16_t>(packet.GetType());
    writeStream.SerializeBits(packetType, 16);

    if (packet.GetChannel() != ePacketChannel::SYSTEM)  // <=v0.2.2-alpha compatibility
    {
        packet.serverTime = g_serverTime;
        writeStream.SerializeBits(packet.serverTime, 32);
    }

    if (!packet.SerializeWrite(writeStream))
    {
        logger::warn("SerializeWrite (packet type #%d) returned false", packetType);
        return;
    }
    writeStream.Flush();

    ePacketChannel packetChannel = packet.GetChannel();
    ePacketReliability packetReliability = GetChannelReliability(packetChannel);

    CNetwork::SendPacket(writeStream.GetData(), writeStream.GetBytesProcessed(), packetChannel, packetReliability);
}

void CPacketFactory::Receive(const uint8_t* data, int dataSize)
{
    serialize::ReadStream readStream = serialize::ReadStream(data, dataSize);

    uint32_t tempPacketType = 0;
    if (!readStream.SerializeBits(tempPacketType, 16))
    {
        logger::warn("SerializeBits couldn't read tempPacketType");
        return;
    }
    ePacketType packetType = static_cast<ePacketType>(tempPacketType);

    Packet* pProtPacket = m_map[packetType];
    if (pProtPacket == nullptr)
    {
        logger::warn("Couldn't find packet instance of #%d", packetType);
        return;
    }

    server_time_t serverTime = 0;
    if (pProtPacket->GetChannel() != ePacketChannel::SYSTEM)  // <=v0.2.2-alpha compatibility
    {
        if (!readStream.SerializeBits(serverTime, 32))
        {
            logger::warn("SerializeBits couldn't read serverTime");
            return;
        }
    }

    if (!pProtPacket->SerializeRead(readStream))
    {
        logger::warn("SerializeRead (packet type #%d) returned false", packetType);
        return;
    }

    Packet* pPacket = pProtPacket->Clone();  // will be deleted in the packet buffer later
                                             // TODO: find a better solution to allocate packets and dont use the heap
    pPacket->serverTime = serverTime;
    GetPacketBuffer().Receive(pPacket);
}

void CPacketFactory::RegisterPacket(Packet* pPacket)
{
    assert(pPacket != nullptr);

    m_map[pPacket->GetType()] = pPacket;
}
