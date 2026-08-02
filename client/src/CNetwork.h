#pragma once
#include "network/packet.h"
#include <cstddef>

class CNetwork
{
public:
    inline static ENetHost* m_pENetHost = nullptr;
    inline static ENetPeer* m_pPeer = nullptr;
    inline static bool m_bConnected = false;
    inline static bool m_bAuthenticated = false;
    inline static char m_IpAddress[128 + 1]{};
    inline static unsigned short m_nPort = Config::DEFAULT_PORT;
    inline static uint32_t ms_nBytesReceivedThisSecond = 0;
    inline static uint32_t ms_nBytesReceivedThisSecondCounter = 0;
    inline static uint32_t ms_nBytesSentThisSecond = 0;
    inline static uint32_t ms_nBytesSentThisSecondCounter = 0;

    static DWORD WINAPI InitAsync(LPVOID);
    static void Disconnect();
    static void ProcessReceive();
    static void ProcessSend();
    static void SendPacket(
        const uint8_t* data, int dataSize, ePacketChannel packetChannel, ePacketReliability packetReliability);

    static bool IsValidIP(const char* ip)
    {
        ENetAddress temp;
        return enet_address_set_host_ip(&temp, ip) >= 0;
    }

    static uint32_t GetSentCount() { return m_pENetHost ? m_pENetHost->totalSentPackets : 0; }
    static uint32_t GetReceivedCount() { return m_pENetHost ? m_pENetHost->totalReceivedPackets : 0; }
    static uint32_t GetRTT() { return m_pPeer ? m_pPeer->roundTripTime : 0; }
    static uint32_t GetRTTVariance() { return m_pPeer ? m_pPeer->roundTripTimeVariance : 0; }
    static uint32_t GetDroppedCount() { return m_pPeer ? m_pPeer->packetsLost : 0; }
    static float GetDroppedRatio() { return m_pPeer ? m_pPeer->packetLoss * 100.0f / 65536.0f : 0.0f; }
};
