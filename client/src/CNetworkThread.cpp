#include "CNetworkThread.h"

void CNetworkThread::Start(ThreadProc threadProc)
{
    if (m_hThread != nullptr)
        return;

    m_bStopRequested = false;
    m_hThread = CreateThread(nullptr, 0, threadProc, nullptr, 0, nullptr);
}

void CNetworkThread::Stop()
{
    RequestStop();

    if (m_hThread != nullptr && GetCurrentThreadId() != GetThreadId(m_hThread))
    {
        WaitForSingleObject(m_hThread, INFINITE);
        CloseHandle(m_hThread);
        m_hThread = nullptr;
    }
}

void CNetworkThread::ClearQueues()
{
    std::lock_guard<std::mutex> lock(m_packetMutex);
    m_outgoingPackets.clear();
    m_incomingPackets.clear();
}

void CNetworkThread::EnqueueOutgoing(QueuedPacket packet)
{
    std::lock_guard<std::mutex> lock(m_packetMutex);
    m_outgoingPackets.push_back(std::move(packet));
}

std::deque<CNetworkThread::QueuedPacket> CNetworkThread::TakeOutgoing()
{
    std::deque<QueuedPacket> packets;
    std::lock_guard<std::mutex> lock(m_packetMutex);
    packets.swap(m_outgoingPackets);
    return packets;
}

void CNetworkThread::EnqueueIncoming(std::vector<uint8_t> packet)
{
    std::lock_guard<std::mutex> lock(m_packetMutex);
    m_incomingPackets.push_back(std::move(packet));
}

std::deque<std::vector<uint8_t>> CNetworkThread::TakeIncoming()
{
    std::deque<std::vector<uint8_t>> packets;
    std::lock_guard<std::mutex> lock(m_packetMutex);
    packets.swap(m_incomingPackets);
    return packets;
}
