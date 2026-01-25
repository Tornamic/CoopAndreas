#pragma once
class CPedDamageResponseInfo
{
public:
    float m_fDamageHealth;
    float m_fDamageArmor;
    bool  m_bHealthZero;
    bool  m_bForceDeath;
    bool  m_bDamageCalculated;
    bool  m_bCheckIfAffectsPed;
};
VALIDATE_SIZE(CPedDamageResponseInfo, 0xC);
