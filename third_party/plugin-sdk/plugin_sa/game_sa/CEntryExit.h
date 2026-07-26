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

class CPed;

struct SEntryExitFlags
{
    SEntryExitFlags()
    {
        bUnknownInterior = false;
        bUnknownPairing = false;
        bCreateLinkedPair = false;
        bRewardInterior = false;
        bUsedRewardEntrance = false;
        bCarsAndAircraft = false;
        bBikesAndMotorcycles = false;
        bDisableOnFoot = false;
        bAcceptNpcGroup = false;
        bFoodDateFlag = false;
        bUnknownBurglary = false;
        bDisableExit = false;
        bBurglaryAccess = false;
        bEnteredWithoutExit = false;
        bEnableAccess = false;
        bDeleteEnex = false;
    }
    unsigned short bUnknownInterior : 1;
    unsigned short bUnknownPairing : 1;
    unsigned short bCreateLinkedPair : 1;
    unsigned short bRewardInterior : 1;
    unsigned short bUsedRewardEntrance : 1;
    unsigned short bCarsAndAircraft : 1;
    unsigned short bBikesAndMotorcycles : 1;
    unsigned short bDisableOnFoot : 1;

    unsigned short bAcceptNpcGroup : 1;
    unsigned short bFoodDateFlag : 1;
    unsigned short bUnknownBurglary : 1;
    unsigned short bDisableExit : 1;
    unsigned short bBurglaryAccess : 1;
    unsigned short bEnteredWithoutExit : 1;
    unsigned short bEnableAccess : 1;
    unsigned short bDeleteEnex : 1;


    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        serialize_bool(stream, bUnknownInterior);
        serialize_bool(stream, bUnknownPairing);
        serialize_bool(stream, bCreateLinkedPair);
        serialize_bool(stream, bRewardInterior);
        serialize_bool(stream, bUsedRewardEntrance);
        serialize_bool(stream, bCarsAndAircraft);
        serialize_bool(stream, bBikesAndMotorcycles);
        serialize_bool(stream, bDisableOnFoot);
        serialize_bool(stream, bAcceptNpcGroup);
        serialize_bool(stream, bFoodDateFlag);
        serialize_bool(stream, bUnknownBurglary);
        serialize_bool(stream, bDisableExit);
        serialize_bool(stream, bBurglaryAccess);
        serialize_bool(stream, bEnteredWithoutExit);
        serialize_bool(stream, bEnableAccess);
        serialize_bool(stream, bDeleteEnex);
        return true;
    }
};
VALIDATE_SIZE(SEntryExitFlags, 0x2);

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
