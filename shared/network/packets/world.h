#pragma once
#include "CWeather.h"
#include <CExplosion.h>
#include "network/serializable_types.h"

namespace Packets::World
{
// TODO: proper weather sync
class GameWeatherTime : public Packet
{
    DEFINE_PACKET_TYPE(GameWeatherTime, ePacketType::GAME_WEATHER_TIME, ePacketChannel::EVENT);

public:
    eWeatherType newWeather;
    eWeatherType oldWeather;
    // eWeatherType forcedWeather;
    uint8_t currentMonth;
    uint8_t currentDay;
    uint8_t currentHour;
    uint8_t currentMinute;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        if (Stream::IsWriting)
        {
            newWeather = VCLAMP(newWeather, WEATHER_EXTRASUNNY_LA, WEATHER_EXTRACOLOURS_2);
            oldWeather = VCLAMP(oldWeather, WEATHER_EXTRASUNNY_LA, WEATHER_EXTRACOLOURS_2);
            // forcedWeather = VCLAMP(forcedWeather, WEATHER_EXTRASUNNY_LA, WEATHER_EXTRACOLOURS_2);
            currentMonth = VCLAMP(currentMonth, 1, 12);
            currentDay = VCLAMP(currentDay, 1, 31);
            currentHour = VCLAMP(currentHour, 0, 23);
            currentMinute = VCLAMP(currentMinute, 0, 59);
        }
        serialize_int(stream, (int&)newWeather, WEATHER_EXTRASUNNY_LA, WEATHER_EXTRACOLOURS_2);
        serialize_int(stream, (int&)oldWeather, WEATHER_EXTRASUNNY_LA, WEATHER_EXTRACOLOURS_2);
        // serialize_int(stream, (int&)forcedWeather, WEATHER_EXTRASUNNY_LA, WEATHER_EXTRACOLOURS_2);

        serialize_int(stream, currentMonth, 1, 12);
        serialize_int(stream, currentDay, 1, 31);
        serialize_int(stream, currentHour, 0, 23);
        serialize_int(stream, currentMinute, 0, 59);

        return true;
    }
};

// unused for now
class AddExplosion : public Packet
{
    DEFINE_PACKET_TYPE(AddExplosion, ePacketType::ADD_EXPLOSION, ePacketChannel::EVENT);

public:
    eExplosionType type;
    WorldPositionCompressed pos;
    int time;
    bool usesSound;
    float cameraShake;
    bool isVisible;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, (int&)type, EXPLOSION_GRENADE, EXPLOSION_RC_VEHICLE);
        serialize_object(stream, pos);
        serialize_int(stream, time, 0, 100);
        serialize_bool(stream, usesSound);
        serialize_compressed_float(stream, cameraShake, -1.0f, 1.0f, 0.01f);
        serialize_bool(stream, isVisible);

        return true;
    }
};

class TagUpdate : public Packet
{
    DEFINE_PACKET_TYPE(TagUpdate, ePacketType::TAG_UPDATE, ePacketChannel::EVENT);

public:
    struct Payload
    {
        int16_t pos_x = 0;
        int16_t pos_y = 0;
        int16_t pos_z = 0;
        bool bFullySprayed = false;
        uint8_t alpha = 0;
    } payload;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, payload.pos_x, -3000, 3000);
        serialize_int(stream, payload.pos_y, -3000, 3000);
        serialize_int(stream, payload.pos_z, -3000, 3000);

        serialize_bool(stream, payload.bFullySprayed);
        if (payload.bFullySprayed)
        {
            payload.alpha = 255;
        }
        else
        {
            if (Stream::IsWriting)
            {
                uint8_t temp = payload.alpha / 8;
                serialize_int(stream, temp, 0, 31);
            }
            else if (Stream::IsReading)
            {
                uint8_t temp;
                serialize_int(stream, temp, 0, 31);
                payload.alpha = temp * 8;
            }
        }

        return true;
    }
};

class UpdateAllTags : public Packet
{
    DEFINE_PACKET_TYPE(UpdateAllTags, ePacketType::UPDATE_ALL_TAGS, ePacketChannel::EVENT);

public:
    TagUpdate::Payload tags[100];

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        for (size_t i = 0; i < ARRAY_SIZE(tags); i++)
        {
            serialize_int(stream, tags[i].pos_x, -3000, 3000);
            serialize_int(stream, tags[i].pos_y, -3000, 3000);
            serialize_int(stream, tags[i].pos_z, -3000, 3000);
            bool bIsNull = false;
            bool bFullySprayed = false;
            if (Stream::IsWriting)
            {
                if (tags[i].alpha == 0)
                {
                    bIsNull = true;
                }
                serialize_bool(stream, bIsNull);
                if (bIsNull)
                {
                    continue;
                }

                if (tags[i].alpha == 255)
                {
                    bFullySprayed = true;
                }
                serialize_bool(stream, bFullySprayed);
                if (bFullySprayed)
                {
                    continue;
                }
                uint8_t temp = tags[i].alpha / 8;
                serialize_int(stream, temp, 0, 31);
            }
            else if (Stream::IsReading)
            {
                serialize_bool(stream, bIsNull);
                if (bIsNull)
                {
                    tags[i].alpha = 0;
                    continue;
                }

                serialize_bool(stream, bFullySprayed);
                if (bFullySprayed)
                {
                    tags[i].alpha = 255;
                    continue;
                }

                uint8_t temp;
                serialize_int(stream, temp, 0, 31);
                tags[i].alpha = temp * 8;
            }
        }
        return true;
    }
};

class UpdateMoonSize : public Packet
{
    DEFINE_PACKET_TYPE(UpdateMoonSize, ePacketType::UPDATE_MOON_SIZE, ePacketChannel::EVENT);

public:
    uint8_t moonSize;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, moonSize, 0, 7);
        return true;
    }
};

// a breakable / movable object of the map (lamp post, petrol pump, bin...) by the dummy it was made from: the model and
// the dummy's position come from the same IPL for everyone, unlike the pool slots
struct WorldObjectKey
{
    static constexpr int32_t MAX_XY = 600000;  // cm
    static constexpr int32_t MIN_Z = -100000;
    static constexpr int32_t MAX_Z = 300000;

    int16_t model = 0;
    int32_t x = 0;
    int32_t y = 0;
    int32_t z = 0;

    bool operator==(const WorldObjectKey& other) const
    {
        return model == other.model && x == other.x && y == other.y && z == other.z;
    }

    CVector GetPosition() const { return CVector(x / 100.0f, y / 100.0f, z / 100.0f); }

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        if (Stream::IsWriting)
        {
            x = VCLAMP(x, -MAX_XY, MAX_XY);
            y = VCLAMP(y, -MAX_XY, MAX_XY);
            z = VCLAMP(z, MIN_Z, MAX_Z);
        }
        serialize_int(stream, model, 0, 32767);
        serialize_int(stream, x, -MAX_XY, MAX_XY);
        serialize_int(stream, y, -MAX_XY, MAX_XY);
        serialize_int(stream, z, MIN_Z, MAX_Z);
        return true;
    }
};

struct WorldObjectKeyHash
{
    size_t operator()(const WorldObjectKey& key) const
    {
        size_t hash = static_cast<uint16_t>(key.model);
        hash = hash * 31 + static_cast<uint32_t>(key.x);
        hash = hash * 31 + static_cast<uint32_t>(key.y);
        hash = hash * 31 + static_cast<uint32_t>(key.z);
        return hash;
    }
};

struct WorldObjectTransform
{
    WorldPositionCompressed pos;
    CVector right;
    CVector forward;  // up = right x forward

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, pos);
        if (Stream::IsWriting)
        {
            right = CVector(VCLAMP(right.x, -1.0f, 1.0f), VCLAMP(right.y, -1.0f, 1.0f), VCLAMP(right.z, -1.0f, 1.0f));
            forward = CVector(VCLAMP(forward.x, -1.0f, 1.0f), VCLAMP(forward.y, -1.0f, 1.0f),
                VCLAMP(forward.z, -1.0f, 1.0f));
        }
        serialize_compressed_float(stream, right.x, -1.0f, 1.0f, 0.0005f);
        serialize_compressed_float(stream, right.y, -1.0f, 1.0f, 0.0005f);
        serialize_compressed_float(stream, right.z, -1.0f, 1.0f, 0.0005f);
        serialize_compressed_float(stream, forward.x, -1.0f, 1.0f, 0.0005f);
        serialize_compressed_float(stream, forward.y, -1.0f, 1.0f, 0.0005f);
        serialize_compressed_float(stream, forward.z, -1.0f, 1.0f, 0.0005f);
        return true;
    }
};

// what happened to the object, see CObject::ObjectDamage. The damage only grows, the transform is the last mover's
struct WorldObjectState
{
    static constexpr uint8_t DAMAGED = 1 << 0;   // m_bRenderDamaged: the damaged model
    static constexpr uint8_t SMASHED = 1 << 1;   // no collision, invisible: smashed or broken into pieces
    static constexpr uint8_t BROKEN = 1 << 2;    // objectFlags.bIsBroken
    static constexpr uint8_t EXPLODED = 1 << 3;  // objectFlags.bIsExploded
    static constexpr uint8_t MOVED = 1 << 4;     // not where its dummy is
    static constexpr uint8_t DAMAGE_MASK = DAMAGED | SMASHED | BROKEN | EXPLODED;

    uint8_t flags = 0;
    WorldObjectTransform transform;  // when MOVED

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_bits(stream, flags, 5);
        if (flags & MOVED)
        {
            serialize_object(stream, transform);
        }
        return true;
    }
};

// client -> server: what changed on its object (live). Server -> client: the same live change, or the stored states of
// the objects around the player when it comes near them (streaming); flags 0 = the object is as on the map again
class WorldObjectStates : public Packet
{
    DEFINE_PACKET_TYPE(WorldObjectStates, ePacketType::WORLD_OBJECT_STATES, ePacketChannel::EVENT);

public:
    static constexpr int MAX_ENTRIES = 32;

    struct Entry
    {
        WorldObjectKey key;
        bool hasTransform = false;  // false: only the damage is reported, the position stays as it was
        WorldObjectState state;
    };

    bool live = false;  // happened just now: play the effects, otherwise apply quietly
    uint8_t count = 0;
    Entry entries[MAX_ENTRIES]{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_bool(stream, live);
        serialize_int(stream, count, 0, MAX_ENTRIES);
        for (int i = 0; i < count; i++)
        {
            serialize_object(stream, entries[i].key);
            serialize_bool(stream, entries[i].hasTransform);
            serialize_object(stream, entries[i].state);
        }
        return true;
    }
};

// the mover of an object sends it while it flies / falls / slides, the rest state follows in WorldObjectStates
class WorldObjectMotion : public Packet
{
    DEFINE_PACKET_TYPE(WorldObjectMotion, ePacketType::WORLD_OBJECT_MOTION, ePacketChannel::SYNC);

public:
    playerid_t mover;  // set by the server: two players who both started it decide by the id
    WorldObjectKey key;
    WorldObjectTransform transform;
    CVector moveSpeed;
    CVector turnSpeed;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_playerid(stream, mover);
        serialize_object(stream, key);
        serialize_object(stream, transform);
        serialize_vector(stream, moveSpeed);
        serialize_vector(stream, turnSpeed);
        return true;
    }
};

}  // namespace Packets::World
