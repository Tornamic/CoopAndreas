#pragma once
#include <CEntryExit.h>
#include <CGame.h>
#include <CRadar.h>
#include <network/CNetworkEntitySerializer.h>

namespace Packets::Scripts
{
class OnMissionFlagSync : public Packet
{
    DEFINE_PACKET_TYPE(OnMissionFlagSync, ePacketType::ON_MISSION_FLAG_SYNC, ePacketChannel::SCRIPT);

public:
    bool bOnMission = false;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_bool(stream, bOnMission);
        return true;
    }
};

struct _EnExPayload
{
    uint8_t areaId = 0;
    SEntryExitFlags flags{};
    int16_t rectLeft = 0;
    int16_t rectBottom = 0;

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, areaId, AREA_MAIN_MAP, MAX_VISIBLE_AREAS - 1);
        serialize_object(stream, flags);
        serialize_int(stream, rectLeft, -3000, 3000);
        serialize_int(stream, rectBottom, -3000, 3000);
        return true;
    }
};

class EnExSync : public Packet
{
    DEFINE_PACKET_TYPE(EnExSync, ePacketType::ENEX_SYNC, ePacketChannel::EVENT);

public:
    static constexpr size_t MAX_ENEX_COUNT = 400;  // max length of the pool, got it here 0x43F927

    bool bDisabled = false;
    bool bBurglary = false;
    int count = 0;
    _EnExPayload enexes[MAX_ENEX_COUNT];

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_bool(stream, bDisabled);
        serialize_bool(stream, bBurglary);
        serialize_int(stream, count, 0, MAX_ENEX_COUNT);
        for (size_t i = 0; i < count; i++)
        {
            serialize_object(stream, enexes[i]);
        }
        return true;
    }
};

#ifdef COOP_SERVER
inline static EnExSync g_lastEnExData{};
inline static CNetworkPlayer* g_pLastEnExPlayerOwner = nullptr;
#endif

class AddMessageGXT : public Packet
{
    DEFINE_PACKET_TYPE(AddMessageGXT, ePacketType::ADD_MESSAGE_GXT, ePacketChannel::SCRIPT);

public:
    enum eGXTMsgType
    {
        sync_COMMAND_PRINT = 0,
        sync_COMMAND_PRINT_BIG,
        sync_COMMAND_PRINT_NOW,
        sync_COMMAND_PRINT_HELP
    };

    playerid_t forWhoPlayerId{};  // TODO(v0.3.1-alpha): dont send s2c
    eGXTMsgType type = sync_COMMAND_PRINT;
    uint32_t time = 0;
    uint8_t flag = 0;
    char gxt[8]{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_playerid(stream, forWhoPlayerId);
        serialize_int(stream, (int&)type, sync_COMMAND_PRINT, sync_COMMAND_PRINT_HELP);
        if (type != sync_COMMAND_PRINT_HELP)
        {
            serialize_uint32(stream, time);
            serialize_uint8(stream, flag);
        }

        serialize_string(stream, gxt, 8);
        gxt[ARRAY_SIZE(gxt) - 1] = '\0';
        return true;
    }
};

class RemoveMessageGXT : public Packet
{
    DEFINE_PACKET_TYPE(RemoveMessageGXT, ePacketType::REMOVE_MESSAGE_GXT, ePacketChannel::SCRIPT);

public:
    playerid_t forWhoPlayerId{};
    char gxt[8]{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_playerid(stream, forWhoPlayerId);

        serialize_string(stream, gxt, 8);
        gxt[ARRAY_SIZE(gxt) - 1] = '\0';
        return true;
    }
};

class StartCutscene : public Packet
{
    DEFINE_PACKET_TYPE(StartCutscene, ePacketType::START_CUTSCENE, ePacketChannel::SCRIPT);

public:
    char name[8];
    eVisibleArea currArea;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_string(stream, name, 8);
        name[ARRAY_SIZE(name) - 1] = '\0';
        serialize_int(stream, (int&)currArea, AREA_MAIN_MAP, MAX_VISIBLE_AREAS - 1);
        return true;
    }
};

class SkipCutscene : public Packet
{
    DEFINE_PACKET_TYPE(SkipCutscene, ePacketType::SKIP_CUTSCENE, ePacketChannel::SCRIPT);

public:
    SenderPlayerId senderPlayerid{};
    int votes = 0;  // temporary unused

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, senderPlayerid);
        serialize_int(stream, votes, 0, Config::MAX_SERVER_PLAYERS);
        return true;
    }
};

class PlayMissionAudio : public Packet
{
    DEFINE_PACKET_TYPE(PlayMissionAudio, ePacketType::PLAY_MISSION_AUDIO, ePacketChannel::SCRIPT);

public:
    uint8_t slotid = 0;
    int audioid = 0;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, slotid, 0, 3);
        serialize_uint32(stream, audioid);
        return true;
    }
};

class TeleportPlayerScripted : public Packet
{
    DEFINE_PACKET_TYPE(TeleportPlayerScripted, ePacketType::TELEPORT_PLAYER_SCRIPTED, ePacketChannel::SCRIPT);

public:
    playerid_t playerid{};
    WorldPositionCompressed pos{};
    RadianAngleCompressed heading{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_playerid(stream, playerid);
        serialize_object(stream, pos);
        serialize_object(stream, heading);
        return true;
    }
};

class OpCodeSync : public Packet
{
    DEFINE_PACKET_TYPE(OpCodeSync, ePacketType::OPCODE_SYNC, ePacketChannel::SCRIPT);

public:
    // this number is the biggest possible serialized opcode in theory, the usual opcode size is ~4-80 bytes
    // sizeof(OpcodeSyncHeader)
    // + NUM_ENTITY_PARAMS * (entity type byte + packed 64-bit entity id)
    // + (NUM_SYNCED_PARAMS - NUM_ENTITY_PARAMS) * sizeof(int)
    // + NUM_SYNCED_PARAMS * sizeof(uint8_t) + NUM_SYNCED_PARAMS * 256
    // (see COpCodeSync.h, only the first 4 parameters can hold an entity)
    static constexpr int MAX_BUFFER_SIZE = 4 + 4 * (sizeof(uint8_t) + sizeof(uint64_t)) + (15 - 4) * sizeof(int) +
                                           15 * sizeof(uint8_t) + 15 * 256;

    int size = 0;
    uint8_t buffer[MAX_BUFFER_SIZE]{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, size, 4, MAX_BUFFER_SIZE);
        serialize_bytes(stream, buffer, size);
        return true;
    }

public:
    std::string ToString() const override
    {
        static char str[32];
        snprintf(str, sizeof(str), "Opcode: 0x%x", *(uint16_t*)&buffer[0]);
        return str;
    }
};

class PerformTaskSequence : public Packet
{
    DEFINE_PACKET_TYPE(PerformTaskSequence, ePacketType::PERFORM_TASK_SEQUENCE, ePacketChannel::SCRIPT);

public:
    // header: playerid_t (filled by the server) + packed pedid_t + uint8_t task count
    static constexpr int HEADER_SIZE = ids::PLAYERID_SIZE + ids::ENTITYID_SIZE + sizeof(uint8_t);
    static constexpr int MAX_BUFFER_SIZE = HEADER_SIZE + 8 * 256;

    int size = 0;
    uint8_t buffer[MAX_BUFFER_SIZE]{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, size, HEADER_SIZE, MAX_BUFFER_SIZE);
        serialize_bytes(stream, buffer, size);
        return true;
    }
};

class CutsceneSkipVote : public Packet
{
    DEFINE_PACKET_TYPE(CutsceneSkipVote, ePacketType::CUTSCENE_SKIP_VOTE, ePacketChannel::SCRIPT);

public:
    SenderPlayerId senderPlayerid{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, senderPlayerid);
        return true;
    }
};

class CutsceneSkipVoteState : public Packet
{
    DEFINE_PACKET_TYPE(CutsceneSkipVoteState, ePacketType::CUTSCENE_SKIP_VOTE_STATE, ePacketChannel::SCRIPT);

public:
    int votes = 0;
    int voters = 0;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, votes, 0, Config::MAX_SERVER_PLAYERS);
        serialize_int(stream, voters, 0, Config::MAX_SERVER_PLAYERS);
        return true;
    }
};

struct _RadarTracePayload
{
    uint16_t counter = 0;
    bool bInUse = false;
    uint32_t colour = 0;
    uint8_t type = BLIP_NONE;
    uint8_t display = BLIP_DISPLAY_NEITHER;
    uint8_t sprite = RADAR_SPRITE_NONE;
    uint8_t coordBlipAppearance = 0;
    bool bBright = false;
    bool bShortRange = false;
    bool bFriendly = false;
    bool bBlipRemain = false;
    bool bBlipFade = false;
    uint16_t size = 0;
    float sphereRadius = 0.0f;
    WorldPositionCompressed position{};  // the entity position for BLIP_OBJECT and BLIP_PICKUP
    CNetworkEntitySerializer entity{};   // BLIP_CAR and BLIP_CHAR only
    bool bHasEntryExit = false;
    int16_t entryExitX = 0;
    int16_t entryExitY = 0;
    playerid_t forWho{};

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_uint16(stream, counter);
        serialize_bool(stream, bInUse);
        if (!bInUse)
        {
            return true;
        }

        serialize_uint32(stream, colour);
        serialize_int(stream, type, BLIP_NONE, BLIP_AIRSTRIP);
        serialize_int(stream, display, BLIP_DISPLAY_NEITHER, BLIP_DISPLAY_BOTH);
        serialize_int(stream, sprite, RADAR_SPRITE_NONE, RADAR_SPRITE_COUNT - 1);
        serialize_int(stream, coordBlipAppearance, 0, 3);
        serialize_bool(stream, bBright);
        serialize_bool(stream, bShortRange);
        serialize_bool(stream, bFriendly);
        serialize_bool(stream, bBlipRemain);
        serialize_bool(stream, bBlipFade);
        serialize_uint16(stream, size);
        serialize_float(stream, sphereRadius);

        if (type == BLIP_CAR || type == BLIP_CHAR)
        {
            serialize_object(stream, entity);
        }
        else
        {
            serialize_object(stream, position);
        }

        serialize_bool(stream, bHasEntryExit);
        if (bHasEntryExit)
        {
            serialize_int(stream, entryExitX, -3000, 3000);
            serialize_int(stream, entryExitY, -3000, 3000);
        }

        serialize_playerid(stream, forWho);
        return true;
    }
};

class RadarTraceUpdate : public Packet
{
    DEFINE_PACKET_TYPE(RadarTraceUpdate, ePacketType::RADAR_TRACE_UPDATE, ePacketChannel::SCRIPT);

public:
    uint8_t index = 0;
    _RadarTracePayload trace{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, index, 0, MAX_RADAR_TRACES - 1);
        serialize_object(stream, trace);
        return true;
    }
};

class UpdateAllRadarTraces : public Packet
{
    DEFINE_PACKET_TYPE(UpdateAllRadarTraces, ePacketType::UPDATE_ALL_RADAR_TRACES, ePacketChannel::SCRIPT);

public:
    _RadarTracePayload traces[MAX_RADAR_TRACES]{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        for (int i = 0; i < MAX_RADAR_TRACES; i++)
        {
            serialize_object(stream, traces[i]);
        }
        return true;
    }
};

}  // namespace Packets::Scripts
