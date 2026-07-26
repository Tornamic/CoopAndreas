#pragma once

#include "network/packet.h"
#include <deque>

class CPacketBuffer
{
public:
    CPacketBuffer(uint32_t delay) : m_delay(delay) {}
    ~CPacketBuffer() {}

    void Receive(Packet* pPacket);
    void Process();

    uint32_t GetRenderTime(server_time_t serverTime) { return serverTime - m_delay; }

    std::deque<Packet*> m_packets;

private:
    uint32_t m_delay;
};

extern inline CPacketBuffer& GetPacketBuffer()
{
    static CPacketBuffer buffer(Config::INTERP_BUFFER_DELAY_MS);
    return buffer;
}
