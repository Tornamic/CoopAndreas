/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PluginBase.h"
#include "RenderWare.h"
#include "CColModel.h"
#include "CEntity.h"

enum eVisibleArea
{
    AREA_MAIN_MAP = 0,
    AREA_HOTEL,
    AREA_MANSION,
    AREA_BANK,
    AREA_MALL,
    AREA_STRIP_CLUB,
    AREA_LAWYERS,
    AREA_COFFEE_SHOP,
    AREA_CONCERT_HALL,
    AREA_STUDIO,
    AREA_RIFLE_RANGE,
    AREA_BIKER_BAR,
    AREA_POLICE_STATION,
    AREA_EVERYWHERE,
    AREA_DIRT,
    AREA_BLOOD,
    AREA_OVALRING,
    AREA_MALIBU_CLUB,
    AREA_PRINT_WORKS,
    MAX_VISIBLE_AREAS
};

class PLUGIN_API CGame {
public:
    //! Current number of area
    SUPPORTED_10US static char(&aDatFile)[32]; // static char aDatFile[32]
    SUPPORTED_10US static int &currLevel;
    SUPPORTED_10US static unsigned char &bMissionPackGame;
    SUPPORTED_10US static int &currArea;
    SUPPORTED_10US static RwMatrix *&m_pWorkingMatrix1;
    SUPPORTED_10US static RwMatrix *&m_pWorkingMatrix2;

    SUPPORTED_10US static bool CanSeeOutSideFromCurrArea();
    SUPPORTED_10US static bool CanSeeWaterFromCurrArea();
    SUPPORTED_10US static void DrasticTidyUpMemory(bool a1);
    SUPPORTED_10US static void FinalShutdown();
    //! dummy function
    SUPPORTED_10US static void GenerateTempPedAtStartOfNetworkGame();
    SUPPORTED_10US static bool Init1(char const *datFile);
    SUPPORTED_10US static bool Init2(char const *datFile);
    SUPPORTED_10US static bool Init3(char const *datFile);
    SUPPORTED_10US static void InitAfterLostFocus();
    SUPPORTED_10US static bool Initialise(char const *datFile);
    SUPPORTED_10US static bool InitialiseCoreDataAfterRW();
    SUPPORTED_10US static bool InitialiseEssentialsAfterRW();
    SUPPORTED_10US static bool InitialiseOnceBeforeRW();
    SUPPORTED_10US static bool InitialiseRenderWare();
    SUPPORTED_10US static void InitialiseWhenRestarting();
    SUPPORTED_10US static void Process();
    SUPPORTED_10US static void ReInitGameObjectVariables();
    //! dummy function
    SUPPORTED_10US static void ReloadIPLs();
    SUPPORTED_10US static void ShutDownForRestart();
    SUPPORTED_10US static bool Shutdown();
    SUPPORTED_10US static void ShutdownRenderWare();
    SUPPORTED_10US static void TidyUpMemory(bool a1, bool clearD3Dmem);
};

SUPPORTED_10US extern int &gameTxdSlot;
extern int &gGameState;

SUPPORTED_10US bool MoveMem(void **pMem);
SUPPORTED_10US bool MoveColModelMemory(CColModel &colModel, bool a2);
SUPPORTED_10US RpGeometry *MoveGeometryMemory(RpGeometry *geometry);
SUPPORTED_10US bool TidyUpModelInfo2(CEntity *entity, bool a2);
SUPPORTED_10US void ValidateVersion();
SUPPORTED_10US void D3DDeviceRestoreCallback();
SUPPORTED_10US bool DoHaspChecks();

#include "meta/meta.CGame.h"
