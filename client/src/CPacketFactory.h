#pragma once
#include "network/packet.h"
#include "network/packet_types.h"
#include <cassert>
#include <unordered_map>
#include <deque>

struct SPacketRecord
{
    ePacketType m_packetType{};
    uint8_t m_bInbound : 1;
    uint8_t m_bOutbound : 1;
    server_time_t m_serverTime{};
    size_t m_nSize = 0;
    std::string m_sContent{};
};

class CPacketFactory
{
public:
    CPacketFactory() {};
    ~CPacketFactory() {};

    void Send(Packet& packet);
    void Receive(const uint8_t* data, int dataSize);
    void RegisterPacket(Packet* pPacket);

    std::unordered_map<ePacketType, Packet*> m_map;

    void AddPacketRecord(const SPacketRecord& packetRecord, const server_time_t serverTime)
    {
        auto& records = m_records[(int)packetRecord.m_packetType];

        while (!records.empty() && serverTime - records.front().m_serverTime > 60000)
        {
            records.pop_front();
        }

        records.emplace_back(packetRecord);
    }
    std::array<std::deque<SPacketRecord>, (int)ePacketType::PACKET_ID_MAX> m_records{};
};

extern inline CPacketFactory& GetPacketFactory()
{
    static CPacketFactory factory;
    return factory;
}
