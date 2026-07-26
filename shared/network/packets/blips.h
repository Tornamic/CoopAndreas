#pragma once

#include <CRadar.h>

namespace Packets::Blips
{
class UpdateEntityBlip : public Packet
{
    DEFINE_PACKET_TYPE(UpdateEntityBlip, ePacketType::UPDATE_ENTITY_BLIP, ePacketChannel::EVENT);

public:
    int forWhoPlayerId = 0; // TODO(v0.3.1-alpha): send C2S only
    CNetworkEntitySerializer entity{};
    bool isFriendly = false;
    eBlipColour color = BLIP_COLOUR_RED;
    eBlipDisplay display = BLIP_DISPLAY_NEITHER;
    uint8_t scale = 0;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, forWhoPlayerId, 0, Config::MAX_SERVER_PLAYERS - 1);
        serialize_object(stream, entity);
        serialize_bool(stream, isFriendly);
        serialize_int(stream, (int&)color, BLIP_COLOUR_RED, BLIP_COLOUR_DESTINATION);
        serialize_int(stream, (int&)display, BLIP_DISPLAY_NEITHER, BLIP_DISPLAY_BOTH);
        serialize_uint8(stream, scale);
        return true;
    }
};

class RemoveEntityBlip : public Packet
{
    DEFINE_PACKET_TYPE(RemoveEntityBlip, ePacketType::REMOVE_ENTITY_BLIP, ePacketChannel::EVENT);

public:
    int forWhoPlayerId = 0; // TODO(v0.3.1-alpha): send C2S only
    CNetworkEntitySerializer entity{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, forWhoPlayerId, 0, Config::MAX_SERVER_PLAYERS - 1);
        serialize_object(stream, entity);
        return true;
    }
};

class ClearEntityBlips : public Packet
{
    DEFINE_PACKET_TYPE(ClearEntityBlips, ePacketType::CLEAR_ENTITY_BLIPS, ePacketChannel::EVENT);

public:
    int forWhoPlayerId = 0; // TODO(v0.3.1-alpha): send C2S only

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, forWhoPlayerId, 0, Config::MAX_SERVER_PLAYERS - 1);
        return true;
    }
};

class UpdateCheckpoint : public Packet
{
    DEFINE_PACKET_TYPE(UpdateCheckpoint, ePacketType::UPDATE_CHECKPOINT, ePacketChannel::EVENT);

public:
    int forWhoPlayerId = 0; // TODO(v0.3.1-alpha): send C2S only
    WorldPositionCompressed position{};
    WorldPositionCompressed radius{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, forWhoPlayerId, 0, Config::MAX_SERVER_PLAYERS - 1);
        serialize_object(stream, position);
        serialize_object(stream, radius);
        return true;
    }
};

class RemoveCheckpoint : public Packet
{
    DEFINE_PACKET_TYPE(RemoveCheckpoint, ePacketType::REMOVE_CHECKPOINT, ePacketChannel::EVENT);

public:
    int forWhoPlayerId = 0; // TODO(v0.3.1-alpha): send C2S only

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, forWhoPlayerId, 0, Config::MAX_SERVER_PLAYERS - 1);
        return true;
    }
};

struct _StaticBlipPayload
{
    WorldPositionCompressed position{};
    uint8_t sprite : 6; // eRadarSprite
    uint8_t display : 2; // eBlipDisplay
    uint8_t type : 1;  // 0 - BLIP_CONTACT_POINT, 1 - BLIP_COORD
    uint8_t trackingBlip : 1; // bool
    uint8_t shortRange : 1; // bool
    uint8_t friendly : 1;   // bool
    uint8_t coordBlipAppearance : 2;  // eCoordBlipAppearance
    uint8_t size : 3; 
    uint8_t color : 4; // eBlipColour
    
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, position);
        serialize_int(stream, sprite, RADAR_SPRITE_NONE, RADAR_SPRITE_COUNT - 1);
        serialize_int(stream, display, BLIP_DISPLAY_NEITHER, BLIP_DISPLAY_BOTH);
        serialize_int(stream, type, 0, 1);
        serialize_bool(stream, trackingBlip);
        serialize_bool(stream, shortRange);
        serialize_bool(stream, friendly);
        serialize_int(stream, coordBlipAppearance, COORD_BLIP_APPEARANCE_NORMAL, COORD_BLIP_APPEARANCE_ENEMY);
        serialize_int(stream, size, 0, 7);
        serialize_int(stream, color, BLIP_COLOUR_RED, BLIP_COLOUR_DESTINATION);
        return true;
    }
};

class StaticBlipsSnapshot : public Packet
{
    DEFINE_PACKET_TYPE(StaticBlipsSnapshot, ePacketType::CREATE_STATIC_BLIP, ePacketChannel::EVENT);

public:
    uint8_t countBlips = 0;
    _StaticBlipPayload blips[MAX_RADAR_TRACES]{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, countBlips, 0, MAX_RADAR_TRACES - 1);
        for (size_t i = 0; i < countBlips; i++)
        {
            serialize_object(stream, blips[i]);
        }
        return true;
    }
};
}  // namespace Packets::Blips
