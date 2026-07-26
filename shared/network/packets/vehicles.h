#pragma once
#include <CVehicle.h>

namespace Packets::Vehicles
{
class VehicleSpawn : public Packet
{
    DEFINE_PACKET_TYPE(VehicleSpawn, ePacketType::VEHICLE_SPAWN, ePacketChannel::EVENT);

public:
    int vehicleid{};
    uint8_t tempid{};
    uint16_t modelid{};
    WorldPositionCompressed pos{};
    RadianAngleCompressed rot{};
    uint8_t color1 = 0;
    uint8_t color2 = 0;
    eVehicleCreatedBy createdBy = RANDOM_VEHICLE;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, vehicleid, 0, Config::MAX_SERVER_VEHICLES - 1);
        serialize_uint8(stream, tempid);
        serialize_int(stream, modelid, MODEL_LANDSTAL, MODEL_UTILTR1);
        serialize_object(stream, pos);
        serialize_object(stream, rot);
        serialize_uint8(stream, color1);
        serialize_uint8(stream, color2);
        serialize_int(stream, (int&)createdBy, RANDOM_VEHICLE, PERMANENT_VEHICLE);
        return true;
    }
};
class VehicleRemove : public Packet
{
    DEFINE_PACKET_TYPE(VehicleRemove, ePacketType::VEHICLE_REMOVE, ePacketChannel::EVENT);

public:
    int vehicleid{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, vehicleid, 0, Config::MAX_SERVER_VEHICLES - 1);
        return true;
    }
};

class VehicleIdleUpdate : public Packet
{
    DEFINE_PACKET_TYPE(VehicleIdleUpdate, ePacketType::VEHICLE_IDLE_UPDATE, ePacketChannel::SYNC);

public:
    int vehicleid{};

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

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, vehicleid, 0, Config::MAX_SERVER_VEHICLES - 1);   

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

        serialize_uint8(stream, color1);
        serialize_uint8(stream, color2);

        if (Stream::IsWriting)
        {
            health = std::clamp(health, 0.0f, 1000.0f);
        }
        serialize_compressed_float(stream, health, 0.0f, 1000.0f, 1.0f);

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

        return true;
    }
};

class VehicleConfirm : public Packet
{
    DEFINE_PACKET_TYPE(VehicleConfirm, ePacketType::VEHICLE_CONFIRM, ePacketChannel::EVENT);

public:
    uint8_t tempid = 0;
    int vehicleid = 0;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_uint8(stream, tempid);
        serialize_int(stream, vehicleid, 0, Config::MAX_SERVER_VEHICLES - 1);
        return true;
    }
};

class VehicleDriverUpdate : public Packet
{
    DEFINE_PACKET_TYPE(VehicleDriverUpdate, ePacketType::VEHICLE_DRIVER_UPDATE, ePacketChannel::SYNC);

public:
    SenderPlayerId playerid{};
    int vehicleid{};
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

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, playerid);
        serialize_int(stream, vehicleid, 0, Config::MAX_SERVER_VEHICLES - 1);
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

        serialize_uint8(stream, color1);
        serialize_uint8(stream, color2);

        if (Stream::IsWriting)
        {
            health = std::clamp(health, 0.0f, 1000.0f);
        }
        serialize_compressed_float(stream, health, 0.0f, 1000.0f, 1.0f);

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

        return true;
    }
};

class VehicleEnter : public Packet
{
    DEFINE_PACKET_TYPE(VehicleEnter, ePacketType::VEHICLE_ENTER, ePacketChannel::EVENT);

public:
    SenderPlayerId playerid{};
    int vehicleid{};
    int8_t seatid = 0;  // 3bits
    bool bForce = false;
    bool bPassenger = false;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, playerid);
        serialize_int(stream, vehicleid, 0, Config::MAX_SERVER_VEHICLES - 1);
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
    SenderPlayerId playerid{};
    bool bForce = false;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, playerid);
        serialize_bool(stream, bForce);

        return true;
    }
};

class VehicleDamage : public Packet
{
    DEFINE_PACKET_TYPE(VehicleDamage, ePacketType::VEHICLE_DAMAGE, ePacketChannel::EVENT);

public:
    int vehicleid{};
    CDamageManager damageManager{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, vehicleid, 0, Config::MAX_SERVER_VEHICLES - 1);
        serialize_object(stream, damageManager);
        return true;
    }
};

class VehicleComponentAdd : public Packet
{
    DEFINE_PACKET_TYPE(VehicleComponentAdd, ePacketType::VEHICLE_COMPONENT_ADD, ePacketChannel::EVENT);

public:
    int vehicleid{};
    int componentid{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, vehicleid, 0, Config::MAX_SERVER_VEHICLES - 1);
        serialize_int(stream, componentid, 0, 19999);

        return true;
    }
};

class VehicleComponentRemove : public Packet
{
    DEFINE_PACKET_TYPE(VehicleComponentRemove, ePacketType::VEHICLE_COMPONENT_REMOVE, ePacketChannel::EVENT);

public:
    int vehicleid{};
    int componentid{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, vehicleid, 0, Config::MAX_SERVER_VEHICLES - 1);
        serialize_int(stream, componentid, 0, 19999);

        return true;
    }
};

class VehiclePassengerUpdate : public Packet
{
    DEFINE_PACKET_TYPE(VehiclePassengerUpdate, ePacketType::VEHICLE_PASSENGER_UPDATE, ePacketChannel::SYNC);
   
public:
    SenderPlayerId playerid{};
    int vehicleid{};
    Packets::Players::SHealthSnapshot playerHealth{};
    Packets::Players::SWeaponSnapshot playerWeapon{};
    Packets::Players::SKeySnapshot playerKeys{};
    bool driveby;
    int8_t seatid;

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_object(stream, playerid);
        serialize_int(stream, vehicleid, 0, Config::MAX_SERVER_VEHICLES - 1);
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
    int vehicleid{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, vehicleid, 0, Config::MAX_SERVER_VEHICLES - 1);

        return true;
    }
};

class SetVehicleCreatedBy : public Packet
{
    DEFINE_PACKET_TYPE(SetVehicleCreatedBy, ePacketType::SET_VEHICLE_CREATED_BY, ePacketChannel::EVENT);

public:
    int vehicleid{};
    eVehicleCreatedBy createdBy{};

private:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_int(stream, vehicleid, 0, Config::MAX_SERVER_VEHICLES - 1);
        serialize_int(stream, (int&)createdBy, RANDOM_VEHICLE, PERMANENT_VEHICLE);

        return true;
    }
};
}  // namespace Packets::Vehicles
