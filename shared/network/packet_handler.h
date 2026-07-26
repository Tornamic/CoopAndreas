#pragma once

#ifdef COOP_SERVER
using packet_handler_t = void (*)(Packet*, CNetworkPlayer* pNetworkPlayer);
#else
using packet_handler_t = void (*)(Packet*);
#endif

class CPacketHandler
{
public:
#ifdef COOP_SERVER
    void ProcessPacket(Packet* pPacket, CNetworkPlayer* pNetworkPlayer)
#else
    void ProcessPacket(Packet* pPacket)
#endif
    {
        assert(pPacket != nullptr);

        ePacketType packetType = pPacket->GetType();
        packet_handler_t handler = m_map[packetType];
        if (handler == nullptr)
        {
            logger::warn("Couldn't find packet handler #%d", packetType);
            return;
        }
#ifdef COOP_SERVER
        handler(pPacket, pNetworkPlayer);
#else
        handler(pPacket);
#endif
    }

    std::unordered_map<ePacketType, packet_handler_t> m_map;
};

extern inline CPacketHandler& GetPacketHandler()
{
    static CPacketHandler handler;
    return handler;
}

class PacketHandlerRegistrar
{
public:
    explicit PacketHandlerRegistrar(ePacketType packetType, packet_handler_t packetHandler)
    {
        GetPacketHandler().m_map[packetType] = packetHandler;
    }
};

#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#define PACKET_HANDLER_IMPL(id, type, ...)                                                             \
    static void CONCAT(_packet_handler_, id)(__VA_ARGS__);                                                     \
    inline static PacketHandlerRegistrar CONCAT(_packet_handler_reg_, id) =                             \
        PacketHandlerRegistrar(type, reinterpret_cast<packet_handler_t>(CONCAT(_packet_handler_, id))); \
    static void CONCAT(_packet_handler_, id)(__VA_ARGS__)

#define PACKET_HANDLER(type, ...) PACKET_HANDLER_IMPL(__COUNTER__, type, __VA_ARGS__)