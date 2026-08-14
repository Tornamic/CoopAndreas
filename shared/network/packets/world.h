#pragma once
#include "network/packet_types.h"
#include "CWeather.h"
#include <CExplosion.h>

namespace Packets::World
{

class GameWeatherTime : public Packet
{
    DEFINE_PACKET_TYPE(GameWeatherTime, ePacketType::GAME_WEATHER_TIME, ePacketChannel::EVENT);

public:
    eWeatherType newWeather{};
    eWeatherType oldWeather{};
    int16_t forcedWeather = -1;
    float interpolationValue = 0.0f;
    float rain = 0.0f;
    float wetRoads = 0.0f;
    float wind = 0.0f;
    bool lightningFlash = false;
    bool lightningBurst = false;
    uint32_t lightningDuration = 0;
    int32_t lightningSoundDelay = -1;
    uint8_t currentMonth = 1;
    uint8_t currentDay = 1;
    uint8_t currentHour = 0;
    uint8_t currentMinute = 0;
    uint8_t currentSecond = 0;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        if (Stream::IsWriting)
        {
            newWeather = VCLAMP(newWeather, WEATHER_EXTRASUNNY_LA, WEATHER_EXTRACOLOURS_2);

            oldWeather = VCLAMP(oldWeather, WEATHER_EXTRASUNNY_LA, WEATHER_EXTRACOLOURS_2);

            forcedWeather = static_cast<int16_t>(
                VCLAMP(static_cast<int>(forcedWeather), -1, static_cast<int>(WEATHER_EXTRACOLOURS_2)));

            interpolationValue = VCLAMP(interpolationValue, 0.0f, 1.0f);

            rain = VCLAMP(rain, 0.0f, 1.0f);

            wetRoads = VCLAMP(wetRoads, 0.0f, 1.0f);

            wind = VCLAMP(wind, 0.0f, 2.0f);

            currentMonth = VCLAMP(currentMonth, 1, 12);

            currentDay = VCLAMP(currentDay, 1, 31);

            currentHour = VCLAMP(currentHour, 0, 23);

            currentMinute = VCLAMP(currentMinute, 0, 59);

            currentSecond = VCLAMP(currentSecond, 0, 59);
        }

        serialize_int(stream, (int&)newWeather, WEATHER_EXTRASUNNY_LA, WEATHER_EXTRACOLOURS_2);

        serialize_int(stream, (int&)oldWeather, WEATHER_EXTRASUNNY_LA, WEATHER_EXTRACOLOURS_2);

        serialize_int(stream, forcedWeather, -1, static_cast<int>(WEATHER_EXTRACOLOURS_2));

        serialize_float(stream, interpolationValue);

        serialize_float(stream, rain);

        serialize_float(stream, wetRoads);

        serialize_float(stream, wind);

        serialize_bool(stream, lightningFlash);

        serialize_bool(stream, lightningBurst);

        serialize_int(stream, (int&)lightningDuration, 0, 1000000);

        serialize_int(stream, lightningSoundDelay, -1, 60000);

        serialize_int(stream, currentMonth, 1, 12);

        serialize_int(stream, currentDay, 1, 31);

        serialize_int(stream, currentHour, 0, 23);

        serialize_int(stream, currentMinute, 0, 59);

        serialize_int(stream, currentSecond, 0, 59);

        return true;
    }
};

class LightningEvent : public Packet
{
    DEFINE_PACKET_TYPE(LightningEvent, ePacketType::LIGHTNING_EVENT, ePacketChannel::EVENT);

public:
    bool flash = false;
    bool burst = false;
    uint32_t duration = 0;
    int32_t soundDelay = -1;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_bool(stream, flash);

        serialize_bool(stream, burst);

        serialize_int(stream, (int&)duration, 0, 1000000);

        serialize_int(stream, soundDelay, -1, 60000);

        return true;
    }
};

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

}  // namespace Packets::World