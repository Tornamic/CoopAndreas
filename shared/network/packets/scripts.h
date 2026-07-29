#pragma once
#include <CEntryExit.h>
#include <CGame.h>

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

class EnExTransition : public Packet
{
    DEFINE_PACKET_TYPE(EnExTransition, ePacketType::ENEX_TRANSITION, ePacketChannel::EVENT);

public:
    SenderPlayerId playerid{};
    WorldPositionCompressed position{};
    RadianAngleCompressed currentRotation{};
    RadianAngleCompressed aimingRotation{};
    uint8_t enexAreaId = AREA_MAIN_MAP;
    uint8_t playerAreaId = AREA_MAIN_MAP;
    int16_t rectLeft = 0;
    int16_t rectBottom = 0;
    bool bFinished = false;
    bool bUsesDoor = false;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, playerid);
        serialize_object(stream, position);
        serialize_object(stream, currentRotation);
        serialize_object(stream, aimingRotation);
        serialize_int(stream, enexAreaId, AREA_MAIN_MAP, MAX_VISIBLE_AREAS - 1);
        serialize_int(stream, playerAreaId, AREA_MAIN_MAP, MAX_VISIBLE_AREAS - 1);
        serialize_int(stream, rectLeft, -3000, 3000);
        serialize_int(stream, rectBottom, -3000, 3000);
        serialize_bool(stream, bFinished);
        if (!bFinished)
        {
            serialize_bool(stream, bUsesDoor);
        }
        else
        {
            bUsesDoor = false;
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

    int forWhoPlayerId = 0; // TODO(v0.3.1-alpha): dont send s2c
    eGXTMsgType type = sync_COMMAND_PRINT;
    uint32_t time = 0;
    uint8_t flag = 0;
    char gxt[8]{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, forWhoPlayerId, 0, Config::MAX_SERVER_PLAYERS - 1);
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
    int forWhoPlayerId = 0;
    char gxt[8]{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, forWhoPlayerId, 0, Config::MAX_SERVER_PLAYERS - 1);

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
    SenderPlayerId playerid{};
    int votes = 0;  // temporary unused

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, playerid);
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
    int playerid = 0;
    WorldPositionCompressed pos{};
    RadianAngleCompressed heading{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, playerid, 0, Config::MAX_SERVER_PLAYERS - 1);
        serialize_object(stream, pos);
        serialize_object(stream, heading);
        return true;
    }
};

class OpCodeSync : public Packet
{
    DEFINE_PACKET_TYPE(OpCodeSync, ePacketType::OPCODE_SYNC, ePacketChannel::SCRIPT);

public:
    // this number is the biggest possible serialized opcode in theory, the usual opcode size is ~4-50 bytes
    // sizeof(OpcodeSyncHeader) + NUM_SYNCED_PARAMS * sizeof(int) + NUM_SYNCED_PARAMS * sizeof(uint8_t) + NUM_SYNCED_PARAMS * 256
    static constexpr int MAX_BUFFER_SIZE = 4 + 15 * sizeof(int) + 15 * sizeof(uint8_t) + 15 * 256;

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
};


class PerformTaskSequence : public Packet
{
    DEFINE_PACKET_TYPE(PerformTaskSequence, ePacketType::PERFORM_TASK_SEQUENCE, ePacketChannel::SCRIPT);

public:
    static constexpr int MAX_BUFFER_SIZE = 9 + 8 * 256;

    int size = 0;
    uint8_t buffer[MAX_BUFFER_SIZE]{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, size, 9, MAX_BUFFER_SIZE);
        serialize_bytes(stream, buffer, size);
        return true;
    }
};

}  // namespace Packets::Scripts
