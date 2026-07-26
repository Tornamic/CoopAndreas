#pragma once
#include <eModelID.h>
#include <ePedType.h>
#include <CPed.h>
#include <CVehicle.h>
#include <eGlobalSpeechContexts.h>

namespace Packets::Peds
{
class PedSpawn : public Packet
{
    DEFINE_PACKET_TYPE(PedSpawn, ePacketType::PED_SPAWN, ePacketChannel::EVENT);

public:
    int pedid = 0;
    uint8_t tempid = 255;
    eModelID modelId = MODEL_MALE01;
    ePedType pedType = PED_TYPE_CIVMALE;
    WorldPositionCompressed pos{};
    eCharCreatedBy createdBy = MISSION_CHAR;
    char specialModelName[8] = {'\0'};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, pedid, 0, Config::MAX_SERVER_PEDS - 1);
        serialize_uint8(stream, tempid);
        serialize_int(stream, (int&)modelId, MODEL_NULL, MODEL_SPECIAL10);
        serialize_int(stream, (int&)pedType, PED_TYPE_PLAYER1, PED_TYPE_MISSION8);
        serialize_object(stream, pos);
        serialize_int(stream, (int&)createdBy, UNUSED_CHAR, REPLAY_CHAR);
        serialize_string(stream, specialModelName, 8);
        specialModelName[ARRAY_SIZE(specialModelName) - 1] = '\0';
        return true;
    }
};

class PedConfirm : public Packet
{
    DEFINE_PACKET_TYPE(PedConfirm, ePacketType::PED_CONFIRM, ePacketChannel::EVENT);

public:
    uint8_t tempid = 255;
    int pedid = 0;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_uint8(stream, tempid);
        serialize_int(stream, pedid, 0, Config::MAX_SERVER_PEDS - 1);
        return true;
    }
};

class PedRemove : public Packet
{
    DEFINE_PACKET_TYPE(PedRemove, ePacketType::PED_REMOVE, ePacketChannel::EVENT);

public:
    int pedid = 0;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, pedid, 0, Config::MAX_SERVER_PEDS - 1);
        return true;
    }
};

class AssignPedSyncer : public Packet
{
    DEFINE_PACKET_TYPE(AssignPedSyncer, ePacketType::ASSIGN_PED, ePacketChannel::EVENT);

public:
    int pedid;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, pedid, 0, Config::MAX_SERVER_PEDS - 1);
        return true;
    }
};

class PedOnFoot : public Packet
{
    DEFINE_PACKET_TYPE(PedOnFoot, ePacketType::PED_ONFOOT, ePacketChannel::SYNC);

public:
    int pedid = 0;
    WorldPositionCompressed pos{};
    MoveSpeedCompressed velocity{};
    Packets::Players::SHealthSnapshot healthSnapshot{};
    Packets::Players::SWeaponSnapshot weaponSnapshot{};
    RadianAngleCompressed aimingRotation{};
    RadianAngleCompressed currentRotation{};
    RadianAngleCompressed lookDirection{};  // this one isnt needed i think
    eMoveState moveState = PEDMOVE_STILL;
    bool bDucked = false;
    bool bAiming = false;
    uint8_t fightingStyle = 4;
    WorldPositionCompressed weaponAim{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, pedid, 0, Config::MAX_SERVER_PEDS - 1);
        serialize_object(stream, pos);
        // serialize_object(stream, velocity); // not used for now
        serialize_object(stream, healthSnapshot);
        serialize_object(stream, weaponSnapshot);
        serialize_object(stream, aimingRotation);
        serialize_object(stream, currentRotation);
        serialize_object(stream, lookDirection);
        serialize_int(stream, (int&)moveState, PEDMOVE_NONE, PEDMOVE_SPRINT);
        serialize_bool(stream, bDucked);
        serialize_bool(stream, bAiming);
        serialize_uint8(stream, fightingStyle);  // todo
        if (bAiming)
        {
            serialize_object(stream, weaponAim);
        }
        return true;
    }
};

class PedDriverUpdate : public Packet
{
    DEFINE_PACKET_TYPE(PedDriverUpdate, ePacketType::PED_DRIVER_UPDATE, ePacketChannel::SYNC);

public:
    int pedid = 0;
    int vehicleid = 0;
    eVehicleType vehicleSubType = VEHICLE_AUTOMOBILE;

    WorldPositionCompressed pos{};
    NormalizedVector rot{};
    NormalizedVector roll{};
    MoveSpeedCompressed velocity{};
    CVector turnSpeed{};

    uint8_t color1{};
    uint8_t color2{};
    int8_t paintjob{};

    Packets::Players::SHealthSnapshot pedHealth{};
    Packets::Players::SWeaponSnapshot pedWeapon{};

    float health{};

    eDoorLock locked = DOORLOCK_UNLOCKED;
    float gasPedal = 0.0f;
    float breakPedal = 0.0f;
    float steerAngle = 0.0f;

    // subType specific fields
    float bikeLean{};               // bike/bmx
    float controlPedaling{};        // bmx
    float planeGearState{};         // plane
    uint16_t miscComponentAngle{};  // automobile/mtruck/plane

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
#pragma region IDs
        serialize_int(stream, pedid, 0, Config::MAX_SERVER_PEDS - 1);
        serialize_int(stream, vehicleid, 0, Config::MAX_SERVER_VEHICLES - 1);
        serialize_int(stream, (int&)vehicleSubType, VEHICLE_AUTOMOBILE, VEHICLE_TRAILER);
#pragma endregion
#pragma region clamp stuff
        if (Stream::IsWriting)
        {
            turnSpeed.x = std::clamp(turnSpeed.x, -0.5f, 0.5f);
            turnSpeed.y = std::clamp(turnSpeed.y, -0.5f, 0.5f);
            turnSpeed.z = std::clamp(turnSpeed.z, -0.5f, 0.5f);
            health = std::clamp(health, 0.0f, 1000.0f);
            gasPedal = std::clamp(gasPedal, -1.0f, 1.0f);
            breakPedal = std::clamp(breakPedal, -1.0f, 1.0f);
            steerAngle = std::clamp(steerAngle, -1.0f, 1.0f);
            bikeLean = std::clamp(bikeLean, -1.0f, 1.0f);
            controlPedaling = std::clamp(controlPedaling, -5.0f, 5.0f);
        }
#pragma endregion
#pragma region matrix speeds
        serialize_object(stream, pos);
        serialize_object(stream, rot);
        serialize_object(stream, roll);
        serialize_object(stream, velocity);

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

#pragma endregion
#pragma region painjob
        serialize_uint8(stream, color1);
        serialize_uint8(stream, color2);
        if (Stream::IsWriting)
        {
            if (paintjob < -1 || paintjob > 2)  // got limits here https://wiki.multitheftauto.com/wiki/Paintjob
            {
                paintjob = -1;
            }
        }
        serialize_int(stream, paintjob, -1, 2);

#pragma endregion
#pragma region ped health
        {
            serialize_object(stream, pedHealth);
            serialize_object(stream, pedWeapon);
        }
#pragma endregion
#pragma region health doors
        serialize_compressed_float(stream, health, 0.0f, 1000.0f, 1.0f);

        serialize_int(stream, (int&)locked, DOORLOCK_NOT_USED, DOORLOCK_SKIP_SHUT_DOORS);
#pragma endregion
#pragma region pedals
        {
            serialize_compressed_float(stream, gasPedal, -1.0f, 1.0f, 0.01f);
            serialize_compressed_float(stream, breakPedal, -1.0f, 1.0f, 0.01f);
            serialize_compressed_float(stream, steerAngle, -1.0f, 1.0f, 0.01f);
        }
#pragma endregion
#pragma region type specific

        if (vehicleSubType == VEHICLE_BIKE || vehicleSubType == VEHICLE_BMX)
        {
            serialize_compressed_float(stream, bikeLean, -1.0f, 1.0f, 0.01f);
        }
        if (vehicleSubType == VEHICLE_BMX)
        {
            serialize_compressed_float(stream, controlPedaling, -5.0f, 5.0f, 0.01f);
        }
        if (vehicleSubType == VEHICLE_PLANE)
        {
            if (Stream::IsWriting)
            {
                bool temp = planeGearState > 0.0f;
                serialize_bool(stream, temp);
            }
            else if (Stream::IsReading)
            {
                bool temp;
                serialize_bool(stream, temp);
                planeGearState = temp ? 1.0f : 0.0f;
            }
        }
        if (vehicleSubType == VEHICLE_AUTOMOBILE || vehicleSubType == VEHICLE_MTRUCK || vehicleSubType == VEHICLE_PLANE)
        {
            bool syncAngle = false;
            if (Stream::IsWriting && miscComponentAngle != 0)
            {
                syncAngle = true;
            }
            serialize_bool(stream, syncAngle);
            if (syncAngle)
            {
                serialize_uint16(stream, miscComponentAngle);
            }
        }
#pragma endregion

        return true;
    }
};

class PedPassengerSync : public Packet
{
    DEFINE_PACKET_TYPE(PedPassengerSync, ePacketType::PED_PASSENGER_UPDATE, ePacketChannel::SYNC);

public:
    int pedid{};
    int vehicleid{};

    Packets::Players::SHealthSnapshot healthSnapshot{};
    Packets::Players::SWeaponSnapshot weaponSnapshot{};

    int8_t seatid;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, pedid, 0, Config::MAX_SERVER_PEDS - 1);
        serialize_int(stream, vehicleid, 0, Config::MAX_SERVER_VEHICLES - 1);
        serialize_object(stream, healthSnapshot);
        serialize_object(stream, weaponSnapshot);
        serialize_int(stream, seatid, -1, 7);  // TODO test properly TODO(v0.3.1-alpha): limits
        return true;
    }
};

class PedShotSync : public Packet
{
    DEFINE_PACKET_TYPE(PedShotSync, ePacketType::PED_SHOT_SYNC, ePacketChannel::EVENT);

public:
    int pedid{};
    eWeaponType weaponType = WEAPON_UNARMED;
    WorldPositionCompressed origin{};
    WorldPositionCompressed effect{};
    WorldPositionCompressed target{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, pedid, 0, Config::MAX_SERVER_PEDS - 1);
        serialize_int(stream, (int&)weaponType, WEAPON_UNARMED, WEAPON_FLARE);
        serialize_object(stream, origin);
        serialize_object(stream, effect);
        serialize_object(stream, target);
        return true;
    }
};

class PedSay : public Packet
{
    DEFINE_PACKET_TYPE(PedSay, ePacketType::PED_SAY, ePacketChannel::EVENT);

public:
    CNetworkEntitySerializer entity{};
    eGlobalSpeechContexts phraseId = CONTEXT_GLOBAL_NO_SPEECH;
    uint32_t startTimeDelay = 0;
    bool overrideSilence = false;
    bool isForceAudible = false;
    bool isFrontEnd = false;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, entity);
        if (entity.entityType != NETWORK_ENTITY_TYPE_PLAYER && entity.entityType != NETWORK_ENTITY_TYPE_PED)
        {
            return false;
        }
        serialize_int(stream, (int&)phraseId, CONTEXT_GLOBAL_NO_SPEECH + 1, CONTEXT_GLOBAL_END - 1);
        serialize_uint32(stream, startTimeDelay);
        serialize_bool(stream, overrideSilence);
        serialize_bool(stream, isForceAudible);
        serialize_bool(stream, isFrontEnd);
        return true;
    }
};

class PedClaimOnRelease : public Packet
{
    DEFINE_PACKET_TYPE(PedClaimOnRelease, ePacketType::PED_CLAIM_ON_RELEASE, ePacketChannel::EVENT);

public:
    int pedid = 0;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, pedid, 0, Config::MAX_SERVER_PEDS - 1);
        return true;
    }
};

class PedCancelClaim : public Packet
{
    DEFINE_PACKET_TYPE(PedCancelClaim, ePacketType::PED_CANCEL_CLAIM, ePacketChannel::EVENT);

public:
    int pedid = 0;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, pedid, 0, Config::MAX_SERVER_PEDS - 1);
        return true;
    }
};

class PedResetAllClaims : public Packet
{
    DEFINE_PACKET_TYPE(PedResetAllClaims, ePacketType::PED_RESET_ALL_CLAIMS, ePacketChannel::EVENT);

public:
    int pedid = 0;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, pedid, 0, Config::MAX_SERVER_PEDS - 1);
        return true;
    }
};

class PedTakeHost : public Packet
{
    DEFINE_PACKET_TYPE(PedTakeHost, ePacketType::PED_TAKE_HOST, ePacketChannel::EVENT);

public:
    int pedid = 0;
    bool allowReturnToPreviousHost = false;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, pedid, 0, Config::MAX_SERVER_PEDS - 1);
        serialize_bool(stream, allowReturnToPreviousHost);
        return true;
    }
};

}  // namespace Packets::Peds
