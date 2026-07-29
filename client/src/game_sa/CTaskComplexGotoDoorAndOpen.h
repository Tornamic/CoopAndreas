#pragma once

#include <CObject.h>
#include <CTaskComplex.h>
#include <CVector.h>

class PLUGIN_API CTaskComplexGotoDoorAndOpen : public CTaskComplex
{
public:
    explicit CTaskComplexGotoDoorAndOpen(CObject* door);
    CTaskComplexGotoDoorAndOpen(const CVector& start, const CVector& end);

    CObject* m_pObject;
    CVector m_vecStart;
    CVector m_vecEnd;
    uint32_t m_nStartTime;
    uint32_t m_nOffsetTime;
    uint8_t m_nState;
    bool m_bRefreshTime;
    uint8_t m_pad32[2];
    uint8_t m_nFlags;
};

VALIDATE_SIZE(CTaskComplexGotoDoorAndOpen, 0x38);
