#pragma once

#include "config.h"
#include "network/packet.h"
#include "network/packet_types.h"
#include "network/serializable_types.h"
#include "serialize.h"
#include <cassert>

// NOLINTBEGIN(readability-simplify-boolean-expr)
// NOLINTBEGIN(readability-isolate-declaration)

namespace Packets::System
{
class PlayerConnected : public Packet
{
public:
    DEFINE_PACKET_TYPE(PlayerConnected, ePacketType::PLAYER_CONNECTED, ePacketChannel::SYSTEM);
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // *NEVER* TOUCH THIS to keep compatibility with <=v0.2.2-alpha
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    struct Payload
    {
        int playerid;
        bool isAlreadyConnected;
        char name[Config::MAX_NICKNAME_LENGTH + 1];
        uint32_t version;
    } payload;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_bytes(stream, (uint8_t*)&payload.playerid, 4);
        serialize_uint8(stream, payload.isAlreadyConnected);
        serialize_bytes(stream, (uint8_t*)payload.name, Config::MAX_NICKNAME_LENGTH + 1);
        serialize_uint32(stream, payload.version);

        if (Stream::IsReading)
        {
// since we serialize the data without checking them, we have to take care of its validity
#ifdef COOP_CLIENT
            if (payload.playerid < 0 || payload.playerid >= Config::MAX_SERVER_PLAYERS)
            {
                return false;
            }

            if (static_cast<uint8_t>(payload.isAlreadyConnected) != 0 &&
                static_cast<uint8_t>(payload.isAlreadyConnected) != 1)
            {
                payload.isAlreadyConnected = false;
            }
#endif

            payload.name[Config::MAX_NICKNAME_LENGTH] = '\0';
            for (int i = 0; i < Config::MAX_NICKNAME_LENGTH; i++)
            {
                char c = payload.name[i];
                if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && (c < '0' || c > '9') && c != '_' && c != '[' &&
                    c != ']' && c != '\0')
                {
                    payload.name[i] = '_';
                }
            }
        }
        return true;
    }
};

class PlayerHandshake : public Packet
{
    DEFINE_PACKET_TYPE(PlayerHandshake, ePacketType::PLAYER_HANDSHAKE, ePacketChannel::SYSTEM);

public:
    int yourid;

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, yourid, 0, Config::MAX_SERVER_PLAYERS - 1);
        return true;
    }
};
class PlayerDisconnected : public Packet
{
    DEFINE_PACKET_TYPE(PlayerDisconnected, ePacketType::PLAYER_DISCONNECTED, ePacketChannel::SYSTEM);

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // *NEVER* TOUCH THIS to keep backwards compatibility with <=v0.2.2-alpha
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
public:
    enum
    {
        DISCONNECTION_REASON_NOTHING = 0,           // >=v0.3.0-alpha
        DISCONNECTION_REASON_VERSION_MISMATCH = 1,  // MUST be 1 (thx EOS)
        DISCONNECTION_REASON_NAME_TAKEN
    };

    struct Payload
    {
        int playerid;
        uint8_t reason;
        uint32_t version;
    } payload;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_bytes(stream, (uint8_t*)&payload.playerid, 4);
        serialize_uint8(stream, payload.reason);
        serialize_uint32(stream, payload.version);

#ifdef COOP_CLIENT
        if (payload.playerid < -1 || payload.playerid >= Config::MAX_SERVER_PLAYERS)
        {
            return false;
        }
        if (payload.playerid == -1)
        {
            if (payload.reason != DISCONNECTION_REASON_VERSION_MISMATCH &&
                payload.reason != DISCONNECTION_REASON_NAME_TAKEN && payload.reason != DISCONNECTION_REASON_NOTHING)
            {
                payload.reason = DISCONNECTION_REASON_NOTHING;
            }
        }
#endif
        return true;
    }
};

class ChatMessage : public Packet
{
    DEFINE_PACKET_TYPE(ChatMessage, ePacketType::PLAYER_CHAT_MESSAGE, ePacketChannel::SYSTEM);

public:
    SenderPlayerId playerid{};
    wchar_t message[Config::MAX_CHAT_MESSAGE_LENGTH + 1];

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, playerid);
        serialize_wstring(stream, message, Config::MAX_CHAT_MESSAGE_LENGTH+1);

        return true;
    }
};

class ServerTimeRequest : public Packet
{
    DEFINE_PACKET_TYPE(ServerTimeRequest, ePacketType::SERVER_TIME_REQUEST, ePacketChannel::SYSTEM);

public:
    server_time_t serverTimeRespond = 0;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        if (Stream::IsWriting)
        {
// dont write anything as a client
#ifdef COOP_CLIENT
            return true;
#endif
        }

        if (Stream::IsReading)
        {
// dont read anything as a server
#ifdef COOP_SERVER
            return true;
#endif
        }

        serialize_uint32(stream, serverTimeRespond);
        return true;
    }
};

class PlayerAssignHost : public Packet
{
    DEFINE_PACKET_TYPE(PlayerAssignHost, ePacketType::PLAYER_ASSIGN_HOST, ePacketChannel::SYSTEM);
public:
    int playerid;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, playerid, 0, Config::MAX_SERVER_PLAYERS - 1);
        return true;
    }
};

class PlayerPing : public Packet
{
    DEFINE_PACKET_TYPE(PlayerPing, ePacketType::PLAYER_PING, ePacketChannel::SYSTEM);

public:
    int playerCount = 0;
    uint16_t ping[Config::MAX_SERVER_PLAYERS]{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, playerCount, 0, Config::MAX_SERVER_PLAYERS);
        for (int i = 0; i < playerCount; i++)
        {
            serialize_uint16(stream, ping[i]);
        }
        return true;
    }
};

}  // namespace Packets::System

// NOLINTEND(readability-isolate-declaration)
// NOLINTEND(readability-simplify-boolean-expr)
