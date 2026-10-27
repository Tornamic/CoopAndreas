/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PluginBase.h"
#include "CRect.h"
#include "CVector.h"
#include "CObject.h"
#include "SEntryExitFlags.h"

class CPed;

class PLUGIN_API CEntryExit {
    PLUGIN_NO_DEFAULT_CONSTRUCTION(CEntryExit)

public:
    char m_szName[8];
    CRect m_recEntrance;
    float m_fEntranceZ;
    float m_fEntranceAngle;
    CVector m_vecExitPos;
    float m_fExitAngle;
    SEntryExitFlags m_nFlags;
    unsigned char m_nArea;
    unsigned char m_nSkyColor;
    unsigned char m_nTimeOn;
    unsigned char m_nTimeOff;
    unsigned char m_nNumberOfPeds;
private:
    char _pad37;
public:
    CEntryExit *m_pLink;

    SUPPORTED_10US static bool &ms_bWarping;
    SUPPORTED_10US static CObject *&ms_pDoor;
    SUPPORTED_10US static CEntryExit *&ms_spawnPoint;

    SUPPORTED_10US void GenerateAmbientPeds(CVector const &position);
    SUPPORTED_10US char *GetEntryExitToDisplayNameOf();
    SUPPORTED_10US void GetPositionRelativeToOutsideWorld(CVector &positionInOut);
    SUPPORTED_10US bool IsInArea(CVector const &position);
    SUPPORTED_10US void RequestAmbientPeds();
    SUPPORTED_10US void RequestObjectsInFrustum();
    SUPPORTED_10US bool TransitionFinished(CPed *player);
    SUPPORTED_10US bool TransitionStarted(CPed *player);
    SUPPORTED_10US void WarpGangWithPlayer(CPed *player);
};

VALIDATE_SIZE(CEntryExit, 0x3C);
VALIDATE_OFFSET(CEntryExit, m_nFlags, 0x30);
VALIDATE_OFFSET(CEntryExit, m_nArea, 0x32);

#include "meta/meta.CEntryExit.h"
