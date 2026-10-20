#pragma once
#include "network/serializable_types.h"
#include <network/eNetworkEntityType.h>
#include <network/CNetworkEntitySerializer.h>

namespace Packets::Objects
{
struct SObjectDamageState
{
    float health = 1000.0f;
    uint8_t lastDamagedWeaponType = 0xFF;  // `CObject::m_nLastWeaponDamage`, 0xFF - not damaged
    bool bRenderDamaged = false;
    bool bIsVisible = true;
    bool bUsesCollision = true;
    bool bHasBeenShattered = false;
    bool bHasExploded = false;

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_float(stream, health);
        serialize_uint8(stream, lastDamagedWeaponType);
        serialize_bool(stream, bRenderDamaged);
        serialize_bool(stream, bIsVisible);
        serialize_bool(stream, bUsesCollision);
        serialize_bool(stream, bHasBeenShattered);
        serialize_bool(stream, bHasExploded);
        return true;
    }

    bool operator==(const SObjectDamageState& rhs) const
    {
        return health == rhs.health && lastDamagedWeaponType == rhs.lastDamagedWeaponType &&
               bRenderDamaged == rhs.bRenderDamaged && bIsVisible == rhs.bIsVisible &&
               bUsesCollision == rhs.bUsesCollision && bHasBeenShattered == rhs.bHasBeenShattered &&
               bHasExploded == rhs.bHasExploded;
    }
    bool operator!=(const SObjectDamageState& rhs) const { return !(*this == rhs); }
};

// SCRIPT channel: must arrive before the synced opcodes that use the object
class ObjectSpawn : public Packet
{
    DEFINE_PACKET_TYPE(ObjectSpawn, ePacketType::OBJECT_SPAWN, ePacketChannel::SCRIPT);

public:
    objectid_t objectid{};
    uint16_t modelid{};
    WorldPositionCompressed pos{};
    NormalizedVector right{};
    NormalizedVector up{};
    uint8_t areaCode = 0;
    SObjectDamageState damageState{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_objectid(stream, objectid);
        serialize_uint16(stream, modelid);
        serialize_object(stream, pos);
        serialize_object(stream, right);
        serialize_object(stream, up);
        serialize_uint8(stream, areaCode);
        serialize_object(stream, damageState);
        return true;
    }
};

class ObjectRemove : public Packet
{
    DEFINE_PACKET_TYPE(ObjectRemove, ePacketType::OBJECT_REMOVE, ePacketChannel::SCRIPT);

public:
    objectid_t objectid{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_objectid(stream, objectid);
        return true;
    }
};

struct SObjectAttachState
{
    bool bAttached = false;
    CNetworkEntitySerializer attachedTo{};
    CVector offset{};        // `m_vecAttachOffset`
    CVector tilt{};          // `m_vecAttachedEntityPosn`
    CVector rotationImag{};  // `m_qAttachedEntityRotation`
    float rotationReal = 0.0f;

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_bool(stream, bAttached);
        if (bAttached)
        {
            serialize_object(stream, attachedTo);
            serialize_vector(stream, offset);
            serialize_vector(stream, tilt);
            serialize_vector(stream, rotationImag);
            serialize_float(stream, rotationReal);
        }
        return true;
    }
};

class ObjectUpdate : public Packet
{
    DEFINE_PACKET_TYPE(ObjectUpdate, ePacketType::OBJECT_UPDATE, ePacketChannel::SYNC);

public:
    objectid_t objectid{};
    SObjectAttachState attach{};
    WorldPositionCompressed pos{};
    NormalizedVector right{};
    NormalizedVector up{};
    MoveSpeedCompressed moveSpeed{};
    CVector turnSpeed{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_objectid(stream, objectid);
        serialize_object(stream, attach);
        serialize_object(stream, pos);
        serialize_object(stream, right);
        serialize_object(stream, up);
        serialize_object(stream, moveSpeed);
        if (Stream::IsWriting)
        {
            turnSpeed.x = std::clamp(turnSpeed.x, -0.5f, 0.5f);
            turnSpeed.y = std::clamp(turnSpeed.y, -0.5f, 0.5f);
            turnSpeed.z = std::clamp(turnSpeed.z, -0.5f, 0.5f);
        }

        bool sendTurnSpeed = false;
        if (Stream::IsWriting)
        {
            if (std::abs(turnSpeed.x) >= 0.0001f || std::abs(turnSpeed.y) >= 0.0001f ||
                std::abs(turnSpeed.z) >= 0.0001f)
            {
                sendTurnSpeed = true;
            }
        }
        serialize_bool(stream, sendTurnSpeed);
        if (sendTurnSpeed)
        {
            serialize_compressed_float(stream, turnSpeed.x, -0.5f, 0.5f, 0.0001f);
            serialize_compressed_float(stream, turnSpeed.y, -0.5f, 0.5f, 0.0001f);
            serialize_compressed_float(stream, turnSpeed.z, -0.5f, 0.5f, 0.0001f);
        }
        else if (Stream::IsReading)
        {
            turnSpeed = CVector(0.0f, 0.0f, 0.0f);
        }
        return true;
    }
};

class ObjectDamageRequest : public Packet
{
    DEFINE_PACKET_TYPE(ObjectDamageRequest, ePacketType::OBJECT_DAMAGE_REQUEST, ePacketChannel::EVENT);

public:
    SenderPlayerId senderPlayerid{};
    objectid_t objectid{};
    float damage = 0.0f;
    uint8_t weapon = 0;
    bool bHasColPos = false;
    WorldPositionCompressed colPos{};
    bool bHasColNormal = false;
    CVector colNormal{};
    bool bInVehicle = false;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, senderPlayerid);
        serialize_objectid(stream, objectid);
        serialize_float(stream, damage);
        serialize_uint8(stream, weapon);
        serialize_bool(stream, bHasColPos);
        if (bHasColPos)
        {
            serialize_object(stream, colPos);
        }
        serialize_bool(stream, bHasColNormal);
        if (bHasColNormal)
        {
            serialize_vector(stream, colNormal);
        }
        serialize_bool(stream, bInVehicle);
        return true;
    }
};

class ObjectDamage : public Packet
{
    DEFINE_PACKET_TYPE(ObjectDamage, ePacketType::OBJECT_DAMAGE, ePacketChannel::EVENT);

public:
    objectid_t objectid{};
    float damage = 0.0f;
    uint8_t weapon = 0;
    bool bHasColPos = false;
    WorldPositionCompressed colPos{};
    bool bHasColNormal = false;
    CVector colNormal{};
    CNetworkEntitySerializer damager{};
    SObjectDamageState state{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_objectid(stream, objectid);
        serialize_float(stream, damage);
        serialize_uint8(stream, weapon);
        serialize_bool(stream, bHasColPos);
        if (bHasColPos)
        {
            serialize_object(stream, colPos);
        }
        serialize_bool(stream, bHasColNormal);
        if (bHasColNormal)
        {
            serialize_vector(stream, colNormal);
        }
        serialize_object(stream, damager);
        serialize_object(stream, state);
        return true;
    }
};
}  // namespace Packets::Objects
