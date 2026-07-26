#pragma once
#include "network/packet.h"
#include "network/packet_types.h"
#include <cassert>
#include <unordered_map>

class CPacketFactory
{
public:
    CPacketFactory() {};
    ~CPacketFactory() {};

    void Send(Packet& packet);
    void Receive(const uint8_t* data, int dataSize);
    void RegisterPacket(Packet* pPacket);

    std::unordered_map<ePacketType, Packet*> m_map;
};

extern inline CPacketFactory& GetPacketFactory()
{
    static CPacketFactory factory;
    return factory;
}