#pragma once
#include "packet_types.h"
#include "serialize.h"

typedef uint32_t server_time_t;

enum class ePacketReliability
{
    RELIABLE,
    UNRELIABLE
};

enum class ePacketChannel : uint8_t
{
    SYSTEM,
    SYNC,
    EVENT,
    SCRIPT,

    COUNT
};

constexpr ePacketReliability aChannelReliability[static_cast<uint8_t>(ePacketChannel::COUNT)] = {
    ePacketReliability::RELIABLE,    // SYSTEM
    ePacketReliability::UNRELIABLE,  // SYNC
    ePacketReliability::RELIABLE,    // EVENT
    ePacketReliability::RELIABLE     // SCRIPT
};

constexpr ePacketReliability GetChannelReliability(ePacketChannel channel)
{
    return aChannelReliability[static_cast<uint8_t>(channel)];
}

template <typename PacketT>
class PacketRegistrar
{
public:
    PacketRegistrar() { GetPacketFactory().RegisterPacket(new PacketT); }
};

#define DEFINE_PACKET_TYPE(packet_class, type, channel)              \
    ePacketType GetType() override                                   \
    {                                                                \
        return type;                                                 \
    }                                                                \
    ePacketChannel GetChannel() override                             \
    {                                                                \
        return channel;                                              \
    }                                                                \
    bool SerializeRead(serialize::ReadStream& stream) override       \
    {                                                                \
        bool result = Serialize(stream);                             \
        if (result)                                                  \
        {                                                            \
            m_bytesRead = stream.GetBytesProcessed();                \
        }                                                            \
        return result;                                               \
    }                                                                \
                                                                     \
    bool SerializeWrite(serialize::WriteStream& stream) override     \
    {                                                                \
        return Serialize(stream);                                    \
    }                                                                \
                                                                     \
    bool SerializeMeasure(serialize::MeasureStream& stream) override \
    {                                                                \
        return Serialize(stream);                                    \
    }                                                                \
    Packet* Clone() const override                                   \
    {                                                                \
        return new packet_class(*this);                              \
    }                                                                \
    inline static PacketRegistrar<packet_class> _registrar

class Packet
{
protected:
    size_t m_bytesRead = 0;

public:
    virtual ~Packet() = default;

    virtual ePacketType GetType() = 0;
    virtual ePacketChannel GetChannel() = 0;
    virtual bool SerializeRead(serialize::ReadStream& stream) = 0;
    virtual bool SerializeWrite(serialize::WriteStream& stream) = 0;
    virtual bool SerializeMeasure(serialize::MeasureStream& stream) = 0;
    virtual Packet* Clone() const = 0;
    virtual std::string ToString() const { return ""; }

    size_t GetBytesRead() { return m_bytesRead; }

    server_time_t serverTime = 0;
};
