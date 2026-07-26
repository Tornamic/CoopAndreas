/*
    Plugin-SDK (Grand Theft Auto San Andreas) source file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "CVector2D.h"
#include "CVector.h"

CVector2D::CVector2D(const CVector& vec3d) {
    x = vec3d.x;
    y = vec3d.y;
}

float CVector2D::Magnitude()
{
	return ((float (__thiscall *)(CVector2D *))0x420860)(this);
}