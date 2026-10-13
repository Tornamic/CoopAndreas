#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <string>

// `playerid_t` is assigned by the server, monotonically increasing during the session,
// starting at 1. 0 (zero) is the invalid player id.
//
// `vehicleid_t` and `pedid_t` are allocated by the client that creates the entity:
// the creator's `playerid_t` plus a per-player, per-entity-type counter starting at 1.
// They are sent as `(uint64_t(creator) << 32) | id` through the network. 0 (zero) is the invalid id.

class playerid_t
{
public:
    playerid_t() : m_value(0) {}
    explicit playerid_t(uint32_t value) : m_value(value) {}

    uint32_t value() const { return m_value; }
    bool valid() const { return m_value != 0; }

    bool operator==(const playerid_t& rhs) const { return m_value == rhs.m_value; }
    bool operator!=(const playerid_t& rhs) const { return m_value != rhs.m_value; }
    bool operator<(const playerid_t& rhs) const { return m_value < rhs.m_value; }

private:
    uint32_t m_value;
};

template <typename Tag>
class entityid_t
{
public:
    entityid_t() : m_creator(), m_id(0) {}
    entityid_t(playerid_t creator, uint32_t id) : m_creator(creator), m_id(id) {}

    static entityid_t FromPacked(uint64_t packed)
    { return entityid_t(playerid_t(static_cast<uint32_t>(packed >> 32)), static_cast<uint32_t>(packed & 0xFFFFFFFFu)); }

    uint64_t packed() const { return (static_cast<uint64_t>(m_creator.value()) << 32) | static_cast<uint64_t>(m_id); }

    // the player that has created (allocated) this entity, not necessarily its current syncer
    playerid_t creator() const { return m_creator; }

    uint32_t id() const { return m_id; }

    bool valid() const { return m_creator.valid() && m_id != 0; }

    // either fully invalid (zero) or fully valid, anything else is malformed
    static bool IsPackedWellFormed(uint64_t packed)
    { return (static_cast<uint32_t>(packed >> 32) == 0) == (static_cast<uint32_t>(packed & 0xFFFFFFFFu) == 0); }

    bool operator==(const entityid_t& rhs) const { return m_creator == rhs.m_creator && m_id == rhs.m_id; }
    bool operator!=(const entityid_t& rhs) const { return !(*this == rhs); }
    bool operator<(const entityid_t& rhs) const { return packed() < rhs.packed(); }

private:
    playerid_t m_creator;
    uint32_t m_id;
};

struct vehicleid_tag
{
};
struct pedid_tag
{
};

typedef entityid_t<vehicleid_tag> vehicleid_t;
typedef entityid_t<pedid_tag> pedid_t;

inline std::string to_string(const playerid_t& playerid)
{
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "p%u", playerid.value());
    return buffer;
}

// "c14v1" - creator's player id, entity-specific id
template <typename Tag>
inline std::string to_string(const entityid_t<Tag>& entityid)
{
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "c%uv%u", entityid.creator().value(), entityid.id());
    return buffer;
}

namespace std
{
template <>
struct hash<playerid_t>
{
    size_t operator()(const playerid_t& playerid) const { return hash<uint32_t>()(playerid.value()); }
};

template <typename Tag>
struct hash<entityid_t<Tag>>
{
    size_t operator()(const entityid_t<Tag>& entityid) const { return hash<uint64_t>()(entityid.packed()); }
};
}  // namespace std

// explicit little-endian byte packing for manually built buffers (opcode/task sequence sync)
namespace ids
{
inline void WriteU32(uint8_t* dst, uint32_t value)
{
    for (int i = 0; i < 4; i++)
    {
        dst[i] = static_cast<uint8_t>(value >> (i * 8));
    }
}

inline uint32_t ReadU32(const uint8_t* src)
{
    uint32_t value = 0;
    for (int i = 0; i < 4; i++)
    {
        value |= static_cast<uint32_t>(src[i]) << (i * 8);
    }
    return value;
}

inline void WriteU64(uint8_t* dst, uint64_t value)
{
    for (int i = 0; i < 8; i++)
    {
        dst[i] = static_cast<uint8_t>(value >> (i * 8));
    }
}

inline uint64_t ReadU64(const uint8_t* src)
{
    uint64_t value = 0;
    for (int i = 0; i < 8; i++)
    {
        value |= static_cast<uint64_t>(src[i]) << (i * 8);
    }
    return value;
}

static const size_t PLAYERID_SIZE = 4;
static const size_t ENTITYID_SIZE = 8;

inline void WritePlayerId(uint8_t* dst, playerid_t playerid)
{ WriteU32(dst, playerid.value()); }

inline playerid_t ReadPlayerId(const uint8_t* src)
{ return playerid_t(ReadU32(src)); }

template <typename Tag>
inline void WriteEntityId(uint8_t* dst, const entityid_t<Tag>& entityid)
{ WriteU64(dst, entityid.packed()); }

// returns an invalid id if the packed value is malformed
template <typename EntityId>
inline EntityId ReadEntityId(const uint8_t* src)
{
    uint64_t packed = ReadU64(src);
    if (!EntityId::IsPackedWellFormed(packed))
    {
        return EntityId();
    }
    return EntityId::FromPacked(packed);
}
}  // namespace ids

#define serialize_playerid(stream, playerid)         \
    do                                               \
    {                                                \
        uint32_t playerid_value = 0;                 \
        if (Stream::IsWriting)                       \
        {                                            \
            playerid_value = (playerid).value();     \
        }                                            \
        serialize_uint32(stream, playerid_value);    \
        if (Stream::IsReading)                       \
        {                                            \
            (playerid) = playerid_t(playerid_value); \
        }                                            \
    } while (0)

#define serialize_entityid_internal(stream, entityid, entityid_type)              \
    do                                                                            \
    {                                                                             \
        uint64_t entityid_packed = 0;                                             \
        if (Stream::IsWriting)                                                    \
        {                                                                         \
            entityid_packed = (entityid).packed();                                \
            serialize_assert(entityid_type::IsPackedWellFormed(entityid_packed)); \
        }                                                                         \
        serialize_uint64(stream, entityid_packed);                                \
        if (Stream::IsReading)                                                    \
        {                                                                         \
            if (!entityid_type::IsPackedWellFormed(entityid_packed))              \
            {                                                                     \
                return false;                                                     \
            }                                                                     \
            (entityid) = entityid_type::FromPacked(entityid_packed);              \
        }                                                                         \
    } while (0)

#define serialize_vehicleid(stream, vehicleid) serialize_entityid_internal(stream, vehicleid, vehicleid_t)
#define serialize_pedid(stream, pedid) serialize_entityid_internal(stream, pedid, pedid_t)
