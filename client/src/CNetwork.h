#pragma once
#include "CNetworkThread.h"
#include "network/packet.h"
#include <cstddef>
#include <atomic>

class CNetwork
{
public:
    inline static ENetHost* m_pENetHost = nullptr;
    inline static ENetPeer* m_pPeer = nullptr;
    inline static std::atomic<bool> m_bConnected = false;
    inline static std::atomic<bool> m_bAuthenticated = false;
    inline static char m_IpAddress[128 + 1]{};
    inline static unsigned short m_nPort = Config::DEFAULT_PORT;
    inline static std::atomic<uint32_t> ms_nBytesReceivedThisSecond = 0;
    inline static std::atomic<uint32_t> ms_nBytesReceivedThisSecondCounter = 0;
    inline static std::atomic<uint32_t> ms_nBytesSentThisSecond = 0;
    inline static std::atomic<uint32_t> ms_nBytesSentThisSecondCounter = 0;
    inline static CNetworkThread m_networkThread;

    static void Start();
    static DWORD WINAPI InitAsync(LPVOID);
    static void Disconnect();
    static void DisconnectOnCrash();
    static void ProcessReceive();
    static void ProcessSend();
    static void SendPacket(
        const uint8_t* data, int dataSize, ePacketChannel packetChannel, ePacketReliability packetReliability);

    static bool IsValidIP(const char* ip)
    {
        ENetAddress temp;
        return enet_address_set_host_ip(&temp, ip) >= 0;
    }

    static uint32_t GetSentCount() { return m_nSentPackets; }
    static uint32_t GetReceivedCount() { return m_nReceivedPackets; }
    static uint32_t GetRTT() { return m_nRTT; }
    static uint32_t GetRTTVariance() { return m_nRTTVariance; }
    static uint32_t GetDroppedCount() { return m_nDroppedPackets; }
    static float GetDroppedRatio() { return m_nPacketLoss * 100.0f / 65536.0f; }

private:
    inline static std::atomic<bool> m_bHandshakePending = false;
    inline static std::atomic<uint32_t> m_nSentPackets = 0;
    inline static std::atomic<uint32_t> m_nReceivedPackets = 0;
    inline static std::atomic<uint32_t> m_nRTT = 0;
    inline static std::atomic<uint32_t> m_nRTTVariance = 0;
    inline static std::atomic<uint32_t> m_nDroppedPackets = 0;
    inline static std::atomic<uint32_t> m_nPacketLoss = 0;
};
