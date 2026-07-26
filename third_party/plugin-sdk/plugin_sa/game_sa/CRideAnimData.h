/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PluginBase.h"

class PLUGIN_API CRideAnimData {
public:
    unsigned int m_nAnimGroup;
    float m_fBarSteerAngle;
    float m_fLeanAngle;
    float m_fDesiredLeanAngle;
    float m_fLeanFwd;
    float m_fAnimLeanLeft;
    float m_fAnimLeanFwd;
};

VALIDATE_SIZE(CRideAnimData, 0x1C);
