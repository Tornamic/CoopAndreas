// Server stand-in for plugin-sdk CVector.h: only what the shared packets need.
// Keep in sync with third_party/plugin-sdk/plugin_sa/game_sa/CVector.h
#pragma once

#include "PluginBase.h"

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

VALIDATE_SIZE(CVector, 0xC);
