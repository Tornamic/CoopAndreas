#pragma once

#include "network/packet.h"
#include <atomic>
#include <deque>
#include <mutex>
#include <vector>
#include <windows.h>

class CNetworkThread
{
public:
    struct QueuedPacket
    {
        std::vector<uint8_t> data;
        ePacketChannel channel;
        ePacketReliability reliability;
    };

    typedef DWORD(WINAPI* ThreadProc)(LPVOID);

    void Start(ThreadProc threadProc);
    void Stop();
    void RequestStop() { m_bStopRequested = true; }
    bool IsStopRequested() const { return m_bStopRequested; }

    void ClearQueues();
    void EnqueueOutgoing(QueuedPacket packet);
    std::deque<QueuedPacket> TakeOutgoing();
    void EnqueueIncoming(std::vector<uint8_t> packet);
    std::deque<std::vector<uint8_t>> TakeIncoming();

private:
    std::mutex m_packetMutex;
    std::deque<QueuedPacket> m_outgoingPackets;
    std::deque<std::vector<uint8_t>> m_incomingPackets;
    HANDLE m_hThread = nullptr;
    std::atomic<bool> m_bStopRequested = false;
};
