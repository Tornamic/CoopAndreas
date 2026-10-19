#pragma once

#include <CRadar.h>

namespace Packets::Blips
{
class UpdateCheckpoint : public Packet
{
    DEFINE_PACKET_TYPE(UpdateCheckpoint, ePacketType::UPDATE_CHECKPOINT, ePacketChannel::EVENT);

public:
    playerid_t forWhoPlayerId{}; // TODO(v0.3.1-alpha): send C2S only
    WorldPositionCompressed position{};
    WorldPositionCompressed radius{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_playerid(stream, forWhoPlayerId);
        serialize_object(stream, position);
        serialize_object(stream, radius);
        return true;
    }
};

class RemoveCheckpoint : public Packet
{
    DEFINE_PACKET_TYPE(RemoveCheckpoint, ePacketType::REMOVE_CHECKPOINT, ePacketChannel::EVENT);

public:
    playerid_t forWhoPlayerId{}; // TODO(v0.3.1-alpha): send C2S only

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_playerid(stream, forWhoPlayerId);
        return true;
    }
};
}  // namespace Packets::Blips
