#pragma once
#include <CVehicle.h>

namespace Packets::Vehicles
{
// CTrain state, the carriages of a train are separate vehicles linked to each other
struct STrainSnapshot
{
public:
    vehicleid_t nextCarriageid{};  // the carriage behind, it is created before this one
    uint8_t trackId = 0;
    bool bIsFrontCarriage = false;
    bool bIsLastCarriage = false;
    bool bMissionTrain = false;
    bool bClockwiseDirection = false;
    bool bStopsAtStations = false;
    bool bForceSlowDown = false;
    bool bDerailed = false;
    float length = 0.0f;  // offset from the carriage in front
    float speed = 0.0f;
    float railDistance = 0.0f;
    uint16_t nodeIndex = 0;
    uint8_t cruiseSpeed = 0;
    uint16_t timeSinceStoppedAtStation = 0;

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        bool hasNextCarriage = false;
        if (Stream::IsWriting)
        {
            hasNextCarriage = nextCarriageid.valid();
        }
        serialize_bool(stream, hasNextCarriage);
        if (hasNextCarriage)
        {
            serialize_vehicleid(stream, nextCarriageid);
        }
        else if (Stream::IsReading)
        {
            nextCarriageid = vehicleid_t{};
        }

        if (Stream::IsWriting)
        {
            length = std::clamp(length, -50.0f, 50.0f);
            speed = std::clamp(speed, -5.0f, 5.0f);
            railDistance = std::clamp(railDistance, 0.0f, 22000.0f);  // CTrainNode stores it as u16 * 3
            timeSinceStoppedAtStation = std::min<uint16_t>(timeSinceStoppedAtStation, 40000);
        }

        serialize_int(stream, trackId, 0, 3);
        serialize_bool(stream, bIsFrontCarriage);
        serialize_bool(stream, bIsLastCarriage);
        serialize_bool(stream, bMissionTrain);
        serialize_bool(stream, bClockwiseDirection);
        serialize_bool(stream, bStopsAtStations);
        serialize_bool(stream, bForceSlowDown);
        serialize_bool(stream, bDerailed);
        serialize_compressed_float(stream, length, -50.0f, 50.0f, 0.001f);
        serialize_compressed_float(stream, speed, -5.0f, 5.0f, 0.0001f);
        serialize_compressed_float(stream, railDistance, 0.0f, 22000.0f, 0.001f);
        serialize_uint16(stream, nodeIndex);
        serialize_uint8(stream, cruiseSpeed);
        serialize_int(stream, timeSinceStoppedAtStation, 0, 40000);
        return true;
    }
};

class VehicleSpawn : public Packet
{
    DEFINE_PACKET_TYPE(VehicleSpawn, ePacketType::VEHICLE_SPAWN, ePacketChannel::EVENT);

public:
    vehicleid_t vehicleid{};
    uint16_t modelid{};
    WorldPositionCompressed pos{};
    RadianAngleCompressed rot{};
    uint8_t color1 = 0;
    uint8_t color2 = 0;
    eVehicleCreatedBy createdBy = RANDOM_VEHICLE;

    bool bIsTrain = false;
    STrainSnapshot train{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_vehicleid(stream, vehicleid);
        serialize_int(stream, modelid, MODEL_LANDSTAL, MODEL_UTILTR1);
        serialize_object(stream, pos);
        serialize_object(stream, rot);
        serialize_uint8(stream, color1);
        serialize_uint8(stream, color2);
        serialize_int(stream, (int&)createdBy, RANDOM_VEHICLE, PERMANENT_VEHICLE);

        serialize_bool(stream, bIsTrain);
        if (bIsTrain)
        {
            serialize_object(stream, train);
        }
        return true;
    }
};
class VehicleRemove : public Packet
{
    DEFINE_PACKET_TYPE(VehicleRemove, ePacketType::VEHICLE_REMOVE, ePacketChannel::EVENT);

public:
    vehicleid_t vehicleid{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_vehicleid(stream, vehicleid);
        return true;
    }
};

class VehicleIdleUpdate : public Packet
{
    DEFINE_PACKET_TYPE(VehicleIdleUpdate, ePacketType::VEHICLE_IDLE_UPDATE, ePacketChannel::SYNC);

public:
    vehicleid_t vehicleid{};
    eVehicleType vehicleSubType = VEHICLE_AUTOMOBILE;

    WorldPositionCompressed pos{};
    NormalizedVector rot{};
    NormalizedVector roll{};
    MoveSpeedCompressed velocity{};
    CVector turnSpeed{};

    uint8_t color1{};
    uint8_t color2{};
    int8_t paintjob{};

    float health{};

    float planeGearState{};
    eDoorLock locked{};

    vehicleid_t trailerid{};

    STrainSnapshot train{};  // train

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_vehicleid(stream, vehicleid);
        serialize_int(stream, (int&)vehicleSubType, VEHICLE_AUTOMOBILE, VEHICLE_TRAILER);

        serialize_object(stream, pos);
        serialize_object(stream, rot);
        serialize_object(stream, roll);
        serialize_object(stream, velocity);
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
            turnSpeed = CVector();
        }

        serialize_uint8(stream, color1);
        serialize_uint8(stream, color2);

        if (Stream::IsWriting)
        {
            health = std::clamp(health, 0.0f, 4000.0f);
        }
        serialize_compressed_float(stream, health, 0.0f, 4000.0f, 1.0f);

        if (Stream::IsWriting)
        {
            if (paintjob < -1 || paintjob > 2)  // got limits here https://wiki.multitheftauto.com/wiki/Paintjob
            {
                paintjob = -1;
            }
        }

        serialize_int(stream, paintjob, -1, 2);

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

        serialize_int(stream, (int&)locked, DOORLOCK_NOT_USED, DOORLOCK_SKIP_SHUT_DOORS);

        bool hasTrailer = false;
        if (Stream::IsWriting)
        {
            hasTrailer = trailerid.valid();
        }
        serialize_bool(stream, hasTrailer);
        if (hasTrailer)
        {
            serialize_vehicleid(stream, trailerid);
        }
        else if (Stream::IsReading)
        {
            trailerid = vehicleid_t{};
        }

        if (vehicleSubType == VEHICLE_TRAIN)
        {
            serialize_object(stream, train);
        }
        return true;
    }
};

class VehicleDriverUpdate : public Packet
{
    DEFINE_PACKET_TYPE(VehicleDriverUpdate, ePacketType::VEHICLE_DRIVER_UPDATE, ePacketChannel::SYNC);

public:
    SenderPlayerId senderPlayerid{};
    vehicleid_t vehicleid{};
    eVehicleType vehicleSubType = VEHICLE_AUTOMOBILE;

    WorldPositionCompressed pos{};
    NormalizedVector rot{};
    NormalizedVector roll{};
    MoveSpeedCompressed velocity{};
    CVector turnSpeed{};

    uint8_t color1{};
    uint8_t color2{};
    int8_t paintjob{};

    Packets::Players::SHealthSnapshot playerHealth{};
    Packets::Players::SWeaponSnapshot playerWeapon{};
    Packets::Players::SKeySnapshot playerKeys{};

    float health{};

    float bikeLean{};
    unsigned short miscComponentAngle{};  // hydra thrusters

    float planeGearState{};
    eDoorLock locked{};

    vehicleid_t trailerid{};

    STrainSnapshot train{};  // train

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, senderPlayerid);
        serialize_vehicleid(stream, vehicleid);
        serialize_int(stream, (int&)vehicleSubType, VEHICLE_AUTOMOBILE, VEHICLE_TRAILER);
        serialize_object(stream, pos);
        serialize_object(stream, rot);
        serialize_object(stream, roll);
        serialize_object(stream, velocity);
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
            turnSpeed = CVector();
        }

        serialize_uint8(stream, color1);
        serialize_uint8(stream, color2);

        if (Stream::IsWriting)
        {
            health = std::clamp(health, 0.0f, 4000.0f);
        }
        serialize_compressed_float(stream, health, 0.0f, 4000.0f, 1.0f);

        serialize_object(stream, playerHealth);
        serialize_object(stream, playerWeapon);
        serialize_object(stream, playerKeys);

        if (Stream::IsWriting)
        {
            if (paintjob < -1 || paintjob > 2)  // got limits here https://wiki.multitheftauto.com/wiki/Paintjob
            {
                paintjob = -1;
            }
        }

        serialize_int(stream, paintjob, -1, 2);

        if (Stream::IsWriting)
        {
            bikeLean = std::clamp(bikeLean, -1.0f, 1.0f);
        }
        serialize_compressed_float(stream, bikeLean, -1.0f, 1.0f, 0.01f);

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
        else if (Stream::IsReading)
        {
            miscComponentAngle = 0;
        }

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

        serialize_int(stream, (int&)locked, DOORLOCK_NOT_USED, DOORLOCK_SKIP_SHUT_DOORS);

        bool hasTrailer = false;
        if (Stream::IsWriting)
        {
            hasTrailer = trailerid.valid();
        }
        serialize_bool(stream, hasTrailer);
        if (hasTrailer)
        {
            serialize_vehicleid(stream, trailerid);
        }
        else if (Stream::IsReading)
        {
            trailerid = vehicleid_t{};
        }

        if (vehicleSubType == VEHICLE_TRAIN)
        {
            serialize_object(stream, train);
        }

        return true;
    }
};

class VehicleEnter : public Packet
{
    DEFINE_PACKET_TYPE(VehicleEnter, ePacketType::VEHICLE_ENTER, ePacketChannel::EVENT);

public:
    SenderPlayerId senderPlayerid{};
    vehicleid_t vehicleid{};
    int8_t seatid = 0;  // 3bits
    bool bForce = false;
    bool bPassenger = false;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, senderPlayerid);
        serialize_vehicleid(stream, vehicleid);
        serialize_int(stream, seatid, -1, 7);  // TODO test properly
        serialize_bool(stream, bForce);
        serialize_bool(stream, bPassenger);

        return true;
    }
};

class VehicleExit : public Packet
{
    DEFINE_PACKET_TYPE(VehicleExit, ePacketType::VEHICLE_EXIT, ePacketChannel::EVENT);

public:
    SenderPlayerId senderPlayerid{};
    bool bForce = false;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, senderPlayerid);
        serialize_bool(stream, bForce);

        return true;
    }
};

class VehicleDamage : public Packet
{
    DEFINE_PACKET_TYPE(VehicleDamage, ePacketType::VEHICLE_DAMAGE, ePacketChannel::EVENT);

public:
    vehicleid_t vehicleid{};
    CDamageManager damageManager{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_vehicleid(stream, vehicleid);
        serialize_object(stream, damageManager);
        return true;
    }
};

class VehicleComponentAdd : public Packet
{
    DEFINE_PACKET_TYPE(VehicleComponentAdd, ePacketType::VEHICLE_COMPONENT_ADD, ePacketChannel::EVENT);

public:
    vehicleid_t vehicleid{};
    int componentid{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_vehicleid(stream, vehicleid);
        serialize_int(stream, componentid, 0, 19999);

        return true;
    }
};

class VehicleComponentRemove : public Packet
{
    DEFINE_PACKET_TYPE(VehicleComponentRemove, ePacketType::VEHICLE_COMPONENT_REMOVE, ePacketChannel::EVENT);

public:
    vehicleid_t vehicleid{};
    int componentid{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_vehicleid(stream, vehicleid);
        serialize_int(stream, componentid, 0, 19999);

        return true;
    }
};

class VehiclePassengerUpdate : public Packet
{
    DEFINE_PACKET_TYPE(VehiclePassengerUpdate, ePacketType::VEHICLE_PASSENGER_UPDATE, ePacketChannel::SYNC);
   
public:
    SenderPlayerId senderPlayerid{};
    vehicleid_t vehicleid{};
    Packets::Players::SHealthSnapshot playerHealth{};
    Packets::Players::SWeaponSnapshot playerWeapon{};
    Packets::Players::SKeySnapshot playerKeys{};
    bool driveby;
    int8_t seatid;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, senderPlayerid);
        serialize_vehicleid(stream, vehicleid);
        serialize_object(stream, playerHealth);
        serialize_object(stream, playerWeapon);
        serialize_object(stream, playerKeys);
        serialize_bool(stream, driveby);
        serialize_int(stream, seatid, -1, 7);  // TODO test properly TODO(v0.3.1-alpha): limits

        return true;
    }
};

class AssignVehicleSyncer : public Packet
{
    DEFINE_PACKET_TYPE(AssignVehicleSyncer, ePacketType::ASSIGN_VEHICLE, ePacketChannel::EVENT);

public:
    vehicleid_t vehicleid{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_vehicleid(stream, vehicleid);

        return true;
    }
};

class SetVehicleCreatedBy : public Packet
{
    DEFINE_PACKET_TYPE(SetVehicleCreatedBy, ePacketType::SET_VEHICLE_CREATED_BY, ePacketChannel::EVENT);

public:
    vehicleid_t vehicleid{};
    eVehicleCreatedBy createdBy{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_vehicleid(stream, vehicleid);
        serialize_int(stream, (int&)createdBy, RANDOM_VEHICLE, PERMANENT_VEHICLE);

        return true;
    }
};
}  // namespace Packets::Vehicles
