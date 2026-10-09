#pragma once
#include "CWeather.h"
#include <CExplosion.h>

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

class PickupCollected : public Packet
{
    DEFINE_PACKET_TYPE(PickupCollected, ePacketType::PICKUP_COLLECTED, ePacketChannel::EVENT);

public:
    uint16_t pickupSlot = 0;
    int16_t pos_x = 0;
    int16_t pos_y = 0;
    int16_t pos_z = 0;
    uint16_t modelId = 0;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, pickupSlot, 0, 619);
        serialize_int(stream, pos_x, -3000, 3000);
        serialize_int(stream, pos_y, -3000, 3000);
        serialize_int(stream, pos_z, -3000, 3000);
        serialize_int(stream, modelId, 0, 20000);
        return true;
    }
};

class UpdateAllPickups : public Packet
{
    DEFINE_PACKET_TYPE(UpdateAllPickups, ePacketType::UPDATE_ALL_PICKUPS, ePacketChannel::EVENT);

public:
    static constexpr int MAX_PICKUPS = 128;

    struct Entry
    {
        uint16_t pickupSlot = 0;
        int16_t pos_x = 0;
        int16_t pos_y = 0;
        int16_t pos_z = 0;
        uint16_t modelId = 0;
    };

    uint16_t count = 0;
    Entry entries[MAX_PICKUPS]{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, count, 0, MAX_PICKUPS);
        for (uint16_t i = 0; i < count; ++i)
        {
            serialize_int(stream, entries[i].pickupSlot, 0, 619);
            serialize_int(stream, entries[i].pos_x, -3000, 3000);
            serialize_int(stream, entries[i].pos_y, -3000, 3000);
            serialize_int(stream, entries[i].pos_z, -3000, 3000);
            serialize_int(stream, entries[i].modelId, 0, 20000);
        }
        return true;
    }
};

class PickupSpawn : public Packet
{
    DEFINE_PACKET_TYPE(PickupSpawn, ePacketType::PICKUP_SPAWN, ePacketChannel::EVENT);

public:
    WorldPositionCompressed pos;
    uint16_t modelId = 0;
    uint8_t pickupType = 0;
    uint16_t ammo = 0;
    bool isEmpty = false;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, pos);
        serialize_int(stream, modelId, 0, 20000);
        serialize_int(stream, pickupType, 0, 31);
        serialize_int(stream, ammo, 0, 50000);
        serialize_bool(stream, isEmpty);
        return true;
    }
};

}  // namespace Packets::World
