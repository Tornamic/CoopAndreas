#pragma once

#include "network/packet.h"
#include "network/packets/system.h"
#include <unordered_map>
class CNetworkPlayer;
class CNetwork
{
public:
    CNetwork() {}

    static bool Init(unsigned short port);
    static void SendPacketNoAuth_ENet(ENetPeer* pENetPeer, const uint8_t* data, int dataSize,
        ePacketChannel packetChannel, ePacketReliability packetReliability);
    static void SendPacket(CNetworkPlayer* pNetworkPlayer, const uint8_t* data, int dataSize,
        ePacketChannel packetChannel, ePacketReliability packetReliability);
    static void SendPacketToAll(const uint8_t* data, int dataSize, ePacketChannel packetChannel,
        ePacketReliability packetReliability, CNetworkPlayer* pNetworkPlayerToIgnore = nullptr);

    static void HandlePlayerConnected(ENetPeer* peer, Packets::System::PlayerConnected& playerConnected);
    ~CNetwork() {}

private:
    static void HandlePeerConnected(ENetEvent& event);
    static void HandlePlayerDisconnected(ENetEvent& event);
};