#include "CServerTime.h"
#include "network/packet.h"
#include "network/packet_types.h"
#include "network/packets/system.h"
#include "stdafx.h"
#include "CPacketBuffer.h"

void CPacketBuffer::Receive(Packet* pPacket)
{
    if (pPacket->GetChannel() == ePacketChannel::SYSTEM)
    {
        GetPacketHandler().ProcessPacket(pPacket);
        delete pPacket;
        return;
    }

    if (m_packets.empty() || pPacket->serverTime >= m_packets.back()->serverTime)
    {
        m_packets.push_back(pPacket);
        return;
    }

    auto it = std::upper_bound(m_packets.begin(), m_packets.end(), pPacket->serverTime,
        [](server_time_t time, const Packet* snapshot) { return time < snapshot->serverTime; });

    m_packets.insert(it, pPacket);
}

void CPacketBuffer::Process()
{
    uint32_t renderTime = GetRenderTime(g_serverTime);

    for (auto it = m_packets.begin(); it != m_packets.end();)
    {
        Packet* pPacket = *it;
        if (pPacket->serverTime > renderTime)
        {
            break;
        }

        it = m_packets.erase(it);

        GetPacketHandler().ProcessPacket(pPacket);
        delete pPacket;
    }
}