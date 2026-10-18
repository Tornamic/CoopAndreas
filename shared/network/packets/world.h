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

// gang zones: how strong every gang is in every zone (CZoneInfo::GangStrength), the radar colours follow from it.
// The host is the authority, see CGangZonesSync
constexpr int MAX_GANG_ZONES = 380; // CTheZones::ZoneInfoArray
constexpr int GANG_COUNT = 10;
constexpr int MAX_GANG_WAR_SPECIFIC_ZONES = 6; // CGangWars::aSpecificZones

class GangZonesUpdate : public Packet
{
    DEFINE_PACKET_TYPE(GangZonesUpdate, ePacketType::GANG_ZONES_UPDATE, ePacketChannel::EVENT);

public:
    struct Zone
    {
        uint16_t index{};
        uint8_t strength[GANG_COUNT]{};
    };

    uint16_t count{};
    Zone zones[MAX_GANG_ZONES]{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, count, 0, MAX_GANG_ZONES);
        for (int i = 0; i < count; i++)
        {
            serialize_int(stream, zones[i].index, 0, MAX_GANG_ZONES - 1);
            for (int gang = 0; gang < GANG_COUNT; gang++)
            {
                serialize_uint8(stream, zones[i].strength[gang]);
            }
        }
        return true;
    }
};

// the script flags that decide where a gang war may start and which zones are coloured on the radar
class GangWarsSettings : public Packet
{
    DEFINE_PACKET_TYPE(GangWarsSettings, ePacketType::GANG_WARS_SETTINGS, ePacketChannel::EVENT);

public:
    bool active{};
    bool radarHidden{}; // the territories are not drawn on the radar
    bool trainingMission{};
    int16_t trainingZone{-1}; // CGangWars::ZoneInfoForTraining
    bool canTriggerOnMission{};
    uint8_t specificZoneCount{};
    int16_t specificZones[MAX_GANG_WAR_SPECIFIC_ZONES]{};

    bool operator==(const GangWarsSettings& other) const
    {
        return active == other.active && radarHidden == other.radarHidden && trainingMission == other.trainingMission &&
               trainingZone == other.trainingZone && canTriggerOnMission == other.canTriggerOnMission &&
               specificZoneCount == other.specificZoneCount &&
               std::equal(specificZones, specificZones + specificZoneCount, other.specificZones);
    }

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_bool(stream, active);
        serialize_bool(stream, radarHidden);
        serialize_bool(stream, trainingMission);
        serialize_int(stream, trainingZone, -1, MAX_GANG_ZONES - 1);
        serialize_bool(stream, canTriggerOnMission);
        serialize_int(stream, specificZoneCount, 0, MAX_GANG_WAR_SPECIFIC_ZONES);
        for (int i = 0; i < specificZoneCount; i++)
        {
            serialize_int(stream, specificZones[i], 0, MAX_GANG_ZONES - 1);
        }
        return true;
    }
};

}  // namespace Packets::World
