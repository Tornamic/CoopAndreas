#pragma once

#define VMIN(a, b) ((a) < (b) ? (a) : (b))
#define VMAX(a, b) ((a) > (b) ? (a) : (b))
#define VCLAMP(v, lo, hi) (VMIN(VMAX((v), (lo)), (hi)))

const float PI = 3.14159265358979323846f;
const float TWO_PI = PI * 2.0f;

// [-pi;pi]
inline static float NormalizeRadianAngle(float angle)
{
    while (angle > PI)
    {
        angle -= TWO_PI;
    }
    while (angle < -PI)
    {
        angle += TWO_PI;
    }
    return angle;
}

inline static float Lerp(float t, float a, float b)
{
    return a + t * (b - a);
}

inline static CVector LerpVector(float t, const CVector& a, const CVector& b)
{
    return a + t * (b - a);
}

inline static float LerpAngle(float t, float a, float b)
{
    float delta = NormalizeRadianAngle(fmod(b - a, TWO_PI));

    float result = a + delta * t;

    result = fmod(result + PI, TWO_PI);
    if (result < 0.0f) result += TWO_PI;
    result -= PI;

    return result;
}