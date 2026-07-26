#pragma once

#include "CPad.h"
#include "CVector.h"
#include "CWeapon.h"
#include "eCamMode.h"
#include "config.h"
#include "eWeaponType.h"
#include "network/packet.h"
#include "math_utils.h"
#include "network/serializable_types.h"
#include "serialize.h"
#include "eTaskType.h"
#include <network/CNetworkEntitySerializer.h>
#include <network/eNetworkEntityType.h>
#include <ePedBones.h>
#include <CPedClothesDesc.h>

// NOLINTBEGIN(readability-simplify-boolean-expr)
// NOLINTBEGIN(readability-isolate-declaration)

namespace Packets::Players
{
struct SKeySnapshot
{
public:
    CControllerStateCompressed newControllerState;
    CControllerStateCompressed oldControllerState;

public:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, newControllerState);
        serialize_object(stream, oldControllerState);
        return true;
    }
};

struct SWeaponSnapshot
{
public:
    uint8_t iWeaponType = WEAPON_UNARMED;
    uint8_t iWeaponState = WEAPONSTATE_READY;
    uint16_t nAmmo = 1;

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, iWeaponType, WEAPON_UNARMED, WEAPON_FLARE);
        serialize_int(stream, iWeaponState, WEAPONSTATE_READY, WEAPONSTATE_MELEE_MADECONTACT);
        bool melee = false;
        if ((iWeaponType >= WEAPON_UNARMED && iWeaponType <= WEAPON_CANE) || iWeaponType == WEAPON_NIGHTVISION ||
            iWeaponType == WEAPON_INFRARED || iWeaponType == WEAPON_PARACHUTE)
        {
            melee = true;
        }
        if (melee)
        {
            if (Stream::IsReading)
            {
                nAmmo = 1;
            }
        }
        else
        {
            serialize_uint16(stream, nAmmo);
        }
        return true;
    }
};

struct SHealthSnapshot
{
public:
    uint8_t iHealth = 100;
    uint8_t iArmour = 0;

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_uint8(stream, iHealth);
        bool hasArmour = false;
        if (Stream::IsWriting)
        {
            if (iArmour != 0)
            {
                hasArmour = true;
            }
        }
        serialize_bool(stream, hasArmour);
        if (hasArmour)
        {
            serialize_uint8(stream, iArmour);
        }
        else
        {
            iArmour = 0;
        }
        return true;
    }
};

class OnFootUpdate : public Packet
{
    DEFINE_PACKET_TYPE(OnFootUpdate, ePacketType::PLAYER_ONFOOT_UPDATE, ePacketChannel::SYNC);

public:
    SenderPlayerId playerid{};
    WorldPositionCompressed vecPos{};
    MoveSpeedCompressed vecMoveSpeed{};
    RadianAngleCompressed currentRotation{};
    RadianAngleCompressed aimingRotation{};
    SHealthSnapshot healthSnapshot{};
    SWeaponSnapshot weaponSnapshot{};
    bool bDucking = false;
    bool bHasJetpack = false;
    int8_t iFightingStyle{};
    SKeySnapshot keySnapshot{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, playerid);

        serialize_object(stream, vecPos);
        serialize_object(stream, vecMoveSpeed);

        serialize_object(stream, currentRotation);
        serialize_object(stream, aimingRotation);

        serialize_object(stream, healthSnapshot);
        serialize_object(stream, weaponSnapshot);

        serialize_bool(stream, bDucking);
        serialize_bool(stream, bHasJetpack);

        serialize_bits(stream, iFightingStyle, 8);  // TODO

        serialize_object(stream, keySnapshot);

        return true;
    }
};

class KeyPressed : public Packet
{
    DEFINE_PACKET_TYPE(KeyPressed, ePacketType::PLAYER_KEY_SYNC, ePacketChannel::SYNC);

public:
    SenderPlayerId playerid{};
    SKeySnapshot keySnapshot;

    RadianAngleCompressed currentRotation{};
    RadianAngleCompressed aimingRotation{};

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, playerid);
        serialize_object(stream, keySnapshot);

        serialize_object(stream, currentRotation);
        serialize_object(stream, aimingRotation);

        return true;
    }
};

class PlayerBulletShot : public Packet
{
    DEFINE_PACKET_TYPE(PlayerBulletShot, ePacketType::PLAYER_BULLET_SHOT, ePacketChannel::EVENT);

public:
    SenderPlayerId playerid{};
    eWeaponType iWeaponType;
    WorldPositionCompressed startPos{};
    WorldPositionCompressed endPos{};
    bool bHitSomething = false;
    bool bHitNetworkEntity = false;
    CNetworkEntitySerializer hitEntity{};
    ePedPieceTypes pedPiece = PED_COL_SPHERE_LEG;
    int incrementalHit = 0;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, playerid);
        serialize_int(stream, (int&)iWeaponType, WEAPON_UNARMED, WEAPON_FLARE);
        serialize_object(stream, startPos);
        serialize_object(stream, endPos);

        serialize_bool(stream, bHitSomething);
        if (bHitSomething)
        {
            serialize_bool(stream, bHitNetworkEntity);
            if (bHitNetworkEntity)
            {
                serialize_object(stream, hitEntity);
                if (hitEntity.entityType == NETWORK_ENTITY_TYPE_PLAYER ||
                    hitEntity.entityType == NETWORK_ENTITY_TYPE_PED)
                {
                    serialize_int(stream, (int&)pedPiece, PED_COL_SPHERE_LEG, PED_SPHERE_HEAD);
                }
            }
        }

        if (iWeaponType == WEAPON_SHOTGUN || iWeaponType == WEAPON_SAWNOFF || iWeaponType == WEAPON_SPAS12)
        {
            serialize_int(stream, incrementalHit, -128, 127);  // TODO: find proper limits
        }
        else
        {
            incrementalHit = 0;
        }

        return true;
    }
};

class PlayerCameraSync : public Packet
{
    DEFINE_PACKET_TYPE(PlayerCameraSync, ePacketType::PLAYER_CAMERA_SYNC, ePacketChannel::SYNC);

public:
    SenderPlayerId playerid{};
    bool bFullUpdate = true;
    uint16_t cameraMode = MODE_NONE;
    float cameraFov = 0.0f;
    NormalizedVector front{};
    WorldPositionCompressed source{};
    NormalizedVector up{};
    float lookPitch = 0.0f;
    RadianAngleCompressed orientation{};

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, playerid);

        serialize_int(stream, cameraMode, MODE_NONE, MODE_AIMWEAPON_ATTACHED);

        serialize_bool(stream, bFullUpdate);
        if (bFullUpdate)
        {
            if (Stream::IsWriting)
            {
                cameraFov = VCLAMP(cameraFov, 0.0f, 255.0f);
            }
            serialize_compressed_float(stream, cameraFov, 0.0f, 255.0f, 1.0f);

            serialize_object(stream, front);
            serialize_object(stream, source);
            serialize_object(stream, up);

            if (Stream::IsWriting)
            {
                lookPitch = NormalizeRadianAngle(lookPitch);
            }
            serialize_compressed_float(stream, lookPitch, -PI, PI, 0.1f);
        }

        if (bFullUpdate)
        {
            if (Stream::IsWriting)
            {
                // dont do anything, orientation will be calculated based on the front vec on reading
            }
            if (Stream::IsReading)
            {
                orientation = atan2f(front.x, front.y);
            }
        }
        else
        {
            serialize_object(stream, orientation);
            if (Stream::IsReading)
            {
                front.x = sinf(orientation.m_angle);
                front.y = cosf(orientation.m_angle);
            }
        }

        return true;
    }

    bool operator==(const PlayerCameraSync& rhs) const
    {
        return cameraMode == rhs.cameraMode && cameraFov == rhs.cameraFov && front == rhs.front &&
               source == rhs.source && up == rhs.up && lookPitch == rhs.lookPitch && orientation == rhs.orientation;
    }

    bool operator!=(const PlayerCameraSync& rhs) const { return !(*this == rhs); }
};

class SetPlayerTask : public Packet
{
    DEFINE_PACKET_TYPE(SetPlayerTask, ePacketType::SET_PLAYER_TASK, ePacketChannel::EVENT);

public:
    SenderPlayerId playerid{};
    int taskType = TASK_NONE;
    WorldPositionCompressed vecPos{};
    RadianAngleCompressed currentRotation{};
    RadianAngleCompressed aimingRotation{};
    bool toggle = false;

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, playerid);

        serialize_int(stream, taskType, TASK_SIMPLE_PLAYER_ON_FOOT, MAX_NUM_TASK_TYPES - 1);

        serialize_object(stream, vecPos);
        serialize_object(stream, currentRotation);
        serialize_object(stream, aimingRotation);
        serialize_bool(stream, toggle);
        return true;
    }
};

class PlayerPlaceWaypoint : public Packet
{
    DEFINE_PACKET_TYPE(PlayerPlaceWaypoint, ePacketType::PLAYER_PLACE_WAYPOINT, ePacketChannel::EVENT);

public:
    SenderPlayerId playerid{};
    bool place = false;
    CVector2D position = CVector2D(0.0f, 0.0f);

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, playerid);

        serialize_bool(stream, place);
        if (place)
        {
            if (Stream::IsWriting)
            {
                position.x = std::clamp(position.x, -3000.0f, 3000.0f);
                position.y = std::clamp(position.y, -3000.0f, 3000.0f);
            }
            serialize_compressed_float(stream, position.x, -3000.0f, 3000.0f, 1.0f);
            serialize_compressed_float(stream, position.y, -3000.0f, 3000.0f, 1.0f);
        }
        return true;
    }
};

class RespawnPlayer : public Packet
{
    DEFINE_PACKET_TYPE(RespawnPlayer, ePacketType::RESPAWN_PLAYER, ePacketChannel::EVENT);

public:
    SenderPlayerId playerid{};

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, playerid);
        return true;
    }
};

class AddProjectile : public Packet
{
    DEFINE_PACKET_TYPE(AddProjectile, ePacketType::ADD_PROJECTILE, ePacketChannel::EVENT);

public:
    CNetworkEntitySerializer creator{};
    eWeaponType projectileType{};
    WorldPositionCompressed origin{};
    float force{};
    bool bDir = false;
    NormalizedVector dir{};
    bool bTarget = false;
    CNetworkEntitySerializer target{};

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, creator);
        serialize_int(stream, (int&)projectileType, WEAPON_UNARMED, WEAPON_FLARE);
        serialize_object(stream, origin);
        
        if (Stream::IsWriting)
        {
            force = std::clamp(force, 0.0f, 1.0f);
        }
        serialize_compressed_float(stream, force, 0.0f, 1.0f, 0.0001f);

        serialize_bool(stream, bDir);
        if (bDir)
        {
            serialize_object(stream, dir);
        }

        serialize_bool(stream, bTarget);
        if (bTarget)
        {
            serialize_object(stream, target);
        }
        return true;
    }
};

class PlayerStats : public Packet
{
    DEFINE_PACKET_TYPE(PlayerStats, ePacketType::PLAYER_STATS, ePacketChannel::EVENT);

public:
    SenderPlayerId playerid{};
    float stats[14]{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, playerid);
        for (size_t i = 0; i < ARRAY_SIZE(stats); i++)
        {
            serialize_float(stream, stats[i]);
        }

        return true;
    }
};

class RebuildPlayer : public Packet
{
    DEFINE_PACKET_TYPE(RebuildPlayer, ePacketType::REBUILD_PLAYER, ePacketChannel::EVENT);

public:
    SenderPlayerId playerid{};
    CPedClothesDesc clothesDesc{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, playerid);
        serialize_object(stream, clothesDesc);

        return true;
    }
};

}  // namespace Packets::Players

// NOLINTEND(readability-isolate-declaration)
// NOLINTEND(readability-simplify-boolean-expr)
