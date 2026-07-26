#pragma once

#include "CPad.h"
#include "CVector.h"
#include "math_utils.h"
#define serialize_vector(stream, vec)   \
    do                                  \
    {                                   \
        serialize_float(stream, vec.x); \
        serialize_float(stream, vec.y); \
        serialize_float(stream, vec.z); \
    } while (0)

struct WorldPositionCompressed : public CVector
{
private:
    static constexpr float WORLD_BOUND_MIN_X = -3000.0f;
    static constexpr float WORLD_BOUND_MAX_X = 3000.0f;
    static constexpr float WORLD_BOUND_MIN_Y = -3000.0f;
    static constexpr float WORLD_BOUND_MAX_Y = 3000.0f;
    static constexpr float WORLD_BOUND_MIN_Z = -120.0f;
    static constexpr float WORLD_BOUND_MAX_Z = 1000.0f;
    static constexpr float PRECISION = 0.001f;

public:
    WorldPositionCompressed() : CVector(0.0f, 0.0f, 0.0f) {};

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        bool compressPos = true;
        if (x < WORLD_BOUND_MIN_X || x > WORLD_BOUND_MAX_X || y < WORLD_BOUND_MIN_Y || y > WORLD_BOUND_MAX_Y ||
            z < WORLD_BOUND_MIN_Z || z > WORLD_BOUND_MAX_Z)
        {
            compressPos = false;
        }

        serialize_bool(stream, compressPos);
        if (compressPos)
        {
            serialize_compressed_float(stream, x, WORLD_BOUND_MIN_X, WORLD_BOUND_MAX_X, PRECISION);
            serialize_compressed_float(stream, y, WORLD_BOUND_MIN_Y, WORLD_BOUND_MAX_Y, PRECISION);
            serialize_compressed_float(stream, z, WORLD_BOUND_MIN_Z, WORLD_BOUND_MAX_Z, PRECISION);
        }
        else
        {
            serialize_float(stream, x);
            serialize_float(stream, y);
            serialize_float(stream, z);
        }
        return true;
    }

    WorldPositionCompressed& operator=(const CVector& vec)
    {
        x = vec.x;
        y = vec.y;
        z = vec.z;
        return *this;
    }

    bool operator==(const WorldPositionCompressed& vec) const
    {
        return x == vec.x && y == vec.y && z == vec.z;
    }
};

struct MoveSpeedCompressed : public CVector
{
private:
    static constexpr float SPEED_MIN = -1.5f;
    static constexpr float SPEED_MAX = 1.5f;
    static constexpr float PRECISION = 0.001f;

public:
    MoveSpeedCompressed() : CVector(0.0f, 0.0f, 0.0f) {};

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        bool isZero = false;
        if (Stream::IsWriting)
        {
            if (abs(x) < PRECISION && abs(y) < PRECISION && abs(z) < PRECISION)
            {
                isZero = true;
            }
        }
        serialize_bool(stream, isZero);
        if (isZero)
        {
            if (Stream::IsReading)
            {
                x = y = z = 0.0f;
            }
            return true;
        }

        bool compressSpeed = true;
        if (x < SPEED_MIN || x > SPEED_MAX || y < SPEED_MIN || y > SPEED_MAX || z < SPEED_MIN || z > SPEED_MAX)
        {
            compressSpeed = false;
        }

        serialize_bool(stream, compressSpeed);
        if (compressSpeed)
        {
            serialize_compressed_float(stream, x, SPEED_MIN, SPEED_MAX, PRECISION);
            serialize_compressed_float(stream, y, SPEED_MIN, SPEED_MAX, PRECISION);
            bool isZZero = false;
            if (Stream::IsWriting)
            {
                if (z == 0.0f)
                {
                    isZZero = true;
                }
            }
            serialize_bool(stream, isZZero);
            if (isZZero)
            {
                if (Stream::IsReading)
                {
                    z = 0.0f;
                }
            }
            else
            {
                serialize_compressed_float(stream, z, SPEED_MIN, SPEED_MAX, PRECISION);
            }
        }
        else
        {
            serialize_float(stream, x);
            serialize_float(stream, y);
            serialize_float(stream, z);
        }
        return true;
    }

    MoveSpeedCompressed& operator=(const CVector& vec)
    {
        x = vec.x;
        y = vec.y;
        z = vec.z;
        return *this;
    }
};

struct CControllerStateCompressed : public CControllerState
{
#define serialize_stick(stream, value)               \
    do                                               \
    {                                                \
        int16_t temp = 0;                             \
                                                     \
        if (Stream::IsWriting)                       \
        {                                            \
            temp = std::clamp<int16_t>(value, -1, 1); \
        }                                            \
                                                     \
        serialize_int(stream, temp, -1, 1);          \
                                                     \
        if (Stream::IsReading)                       \
        {                                            \
            value = temp * 128;                      \
        }                                            \
    } while (false)

#define serialize_bool_key(stream, key)   \
    do                                    \
    {                                     \
        bool temp = false;                \
        if (Stream::IsWriting)            \
        {                                 \
            temp = key > 0;               \
            serialize_bool(stream, temp); \
        }                                 \
        else if (Stream::IsReading)       \
        {                                 \
            serialize_bool(stream, temp); \
            key = temp ? 255 : 0;         \
        }                                 \
    } while (false)

public:
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        if (Stream::IsReading)
        {
            memset(this, 0, sizeof(CControllerStateCompressed));
        }
        serialize_stick(stream, LeftStickX);
        serialize_stick(stream, LeftStickY);
        serialize_bool_key(stream, LeftShoulder1);
        serialize_bool_key(stream, LeftShoulder2);
        serialize_bool_key(stream, RightShoulder1);
        serialize_bool_key(stream, RightShoulder2);
        serialize_bool_key(stream, ButtonSquare);
        serialize_bool_key(stream, ButtonTriangle);
        serialize_bool_key(stream, ButtonCross);
        serialize_bool_key(stream, ButtonCircle);
        serialize_bool_key(stream, ShockButtonL);
        serialize_bool_key(stream, ShockButtonR);
        serialize_bool_key(stream, m_bPedWalk);

        return true;
    }
    CControllerStateCompressed& operator=(const CControllerState& state)
    {
        memcpy(this, &state, sizeof(CControllerState));
        return *this;
    }
#undef serialize_bool_key
#undef serialize_stick
};

struct RadianAngleCompressed
{
    float m_angle = 0.0f;  // [-pi;pi]

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        if (Stream::IsWriting)
        {
            m_angle = NormalizeRadianAngle(m_angle);
        }
        serialize_compressed_float(stream, m_angle, -PI, PI, 0.001f);
        return true;
    }

    RadianAngleCompressed& operator=(const float& angle)
    {
        m_angle = angle;
        return *this;
    }

    bool operator==(const RadianAngleCompressed& angle) const
    {
        return m_angle == angle.m_angle;
    }
};

struct SenderPlayerId
{
public:
    int value = 0;
    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        // do not send player's id C2S, the server knows our ID on its own
        if (Stream::IsWriting && Config::IsClient) 
        {
            return true;
        }
        if (Stream::IsReading && Config::IsServer)
        {
            return true;
        }

        serialize_int(stream, value, 0, Config::MAX_SERVER_PLAYERS - 1);

        return true;
    }

    void operator=(int& right)
    {
        assert(right >= 0 && right < Config::MAX_SERVER_PLAYERS);
        value = right;
    }
};

struct NormalizedVector : public CVector
{
private:
    static constexpr float PRECISION = 0.001f;

public:
    NormalizedVector() : CVector(0.0f, 0.0f, 0.0f) {}

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        
        serialize_compressed_float(stream, x, -1.0f, 1.0f, PRECISION);
        serialize_compressed_float(stream, y, -1.0f, 1.0f, PRECISION);
        serialize_compressed_float(stream, z, -1.0f, 1.0f, PRECISION);
        return true;
    }

    NormalizedVector& operator=(const CVector& vec)
    {
        x = vec.x;
        y = vec.y;
        z = vec.z;
        return *this;
    }

    bool operator==(const NormalizedVector& vec) const
    {
        return x == vec.x && y == vec.y && z == vec.z;
    }
};
