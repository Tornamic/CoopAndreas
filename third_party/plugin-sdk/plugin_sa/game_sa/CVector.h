/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once
#include "PluginBase.h"
#include "RenderWare.h"

class CVector
{
public:
    float x, y, z;

    CVector() { x = y = z = 0.0f; }
    CVector(float X, float Y, float Z)
    {
        x = X;
        y = Y;
        z = Z;
    }

    inline CVector(const CVector& src)
    {
        x = src.x;
        y = src.y;
        z = src.z;
    }

    inline CVector(const RwV3d& right) { FromRwV3d(right); }

    // Returns length of vector
    float Magnitude();

    // Returns length of 2d vector
    float Magnitude2D();

    float MagnitudeSqr2D() const { return x * x + y * y; }

    // Normalises a vector
    void Normalise();

    // Normalises a vector and returns length
    float NormaliseAndMag();

    // Performs cross calculation
    void Cross(const CVector& left, const CVector& right);

    // Adds left + right and stores result
    void Sum(const CVector& left, const CVector& right);

    // Subtracts left - right and stores result
    void Difference(const CVector& left, const CVector& right);

    void operator=(const CVector& right)
    {
        x = right.x;
        y = right.y;
        z = right.z;
    }
    void operator+=(const CVector& right);
    void operator-=(const CVector& right);
    void operator*=(float multiplier);
    void operator/=(float divisor);

    // matrix * vector multiplication
    void FromMultiply(class CMatrix const& matrix, const CVector& vector);
    void FromMultiply3x3(class CMatrix const& matrix, const CVector& vector);

    inline void Set(float X, float Y, float Z)
    {
        x = X;
        y = Y;
        z = Z;
    }

    inline RwV3d ToRwV3d() const { return {x, y, z}; }

    inline void FromRwV3d(const RwV3d& rwvec)
    {
        x = rwvec.x;
        y = rwvec.y;
        z = rwvec.z;
    }

    inline float Heading() { return atan2f(-x, y); }
};

inline CVector operator-(const CVector& vecOne, const CVector& vecTwo)
{
    return CVector(vecOne.x - vecTwo.x, vecOne.y - vecTwo.y, vecOne.z - vecTwo.z);
}

inline CVector operator+(const CVector& vecOne, const CVector& vecTwo)
{
    return CVector(vecOne.x + vecTwo.x, vecOne.y + vecTwo.y, vecOne.z + vecTwo.z);
}

inline CVector operator*(const CVector& vec, float multiplier)
{
    return CVector(vec.x * multiplier, vec.y * multiplier, vec.z * multiplier);
}

inline CVector operator*(float multiplier, const CVector& vec)
{
    return CVector(vec.x * multiplier, vec.y * multiplier, vec.z * multiplier);
}

inline float DistanceBetweenPoints(const CVector& pointOne, const CVector& pointTwo)
{
    CVector diff = pointTwo - pointOne;
    return diff.Magnitude();
}

VALIDATE_SIZE(CVector, 0xC);
