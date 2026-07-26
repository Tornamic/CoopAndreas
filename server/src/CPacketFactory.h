#pragma once

#include "network/packet.h"

class CPacketFactory
{
public:
    CPacketFactory() {};
    ~CPacketFactory() {};

    void Send(Packet& packet, CNetworkPlayer* pNetworkPlayer);
    void SendToAll(Packet& packet, CNetworkPlayer* pNetworkPlayerToIgnore = nullptr);
    void Receive(const uint8_t* data, int dataSize, CNetworkPlayer* pNetworkPlayer);

    void SendPacketNoAuth_ENet(Packet& packet, ENetPeer* pENetPeer);
    void ReceivePacketNoAuth_ENet(const uint8_t* data, int dataSize, ENetPeer* pENetPeer);

    void RegisterPacket(Packet* pPacket);

    std::unordered_map<ePacketType, Packet*> m_map;
};

extern inline CPacketFactory& GetPacketFactory()
{
    static CPacketFactory factory;
    return factory;
}