#pragma once
#include "stdafx.h"
#include "CChat.h"

#pragma pack(1)
typedef struct _CPassengerAim
{
	CVector Front;
	CVector Source;
	CVector SourceBeforeLookBehind;
	CVector Up;

} CPassengerAim;

#pragma pack(1)
struct ÑCompressedControllerState
{
public:
    signed short LeftStickX; // move/steer left (-128?)/right (+128)
    signed short LeftStickY; // move back(+128)/forwards(-128?)

    union
    {
        struct
        {
            unsigned int LeftShoulder1 : 1;
            unsigned int LeftShoulder2 : 1;
            unsigned int RightShoulder1 : 1; // target / hand brake
            unsigned int RightShoulder2 : 1;
            unsigned int DPadUp : 1; // radio change up
            unsigned int DPadDown : 1; // radio change down
            unsigned int DPadLeft : 1; // Skip trip/Action / Negative talk reply
            unsigned int DPadRight : 1; // Next user MP3 track / Positive talk reply
            unsigned int Start : 1; // Pause
            unsigned int Select : 1; // Camera modes
            unsigned int ButtonSquare : 1; // jump / reverse
            unsigned int ButtonTriangle : 1; // get in/out
            unsigned int ButtonCross : 1; // sprint / accelerate
            unsigned int ButtonCircle : 1; // fire
            unsigned int ShockButtonL : 1;
            unsigned int ShockButtonR : 1; // look behind
            unsigned int m_bChatIndicated : 1;
            unsigned int m_bPedWalk : 1;
            unsigned int m_bVehicleMouseLook : 1;
            unsigned int m_bRadioTrackSkip : 1;
            
            unsigned int m_bDisableControls : 1;
        };
        unsigned int compressed; // To access all bits as a single value if needed
    };

    ÑCompressedControllerState()
    {
        LeftStickX = LeftStickY = compressed = 0;
    }

#define COMPRESS(field) field = (state.field > 0)
    ÑCompressedControllerState(const CControllerState& state, bool disableControls)
    {
        LeftStickX = state.LeftStickX;
        LeftStickY = state.LeftStickY;

        COMPRESS(LeftShoulder1);
        COMPRESS(LeftShoulder2);
        COMPRESS(RightShoulder1);
        COMPRESS(RightShoulder2);
        COMPRESS(DPadUp);
        COMPRESS(DPadDown);
        COMPRESS(DPadLeft);
        COMPRESS(DPadRight);
        COMPRESS(Start);
        COMPRESS(Select);
        COMPRESS(ButtonSquare);
        COMPRESS(ButtonTriangle);
        COMPRESS(ButtonCross);
        COMPRESS(ButtonCircle);
        COMPRESS(ShockButtonL);
        COMPRESS(ShockButtonR);
        COMPRESS(m_bChatIndicated);
        COMPRESS(m_bPedWalk);
        COMPRESS(m_bVehicleMouseLook);
        COMPRESS(m_bRadioTrackSkip);
        m_bDisableControls = disableControls;
    }
#undef COMPRESS

#define DECOMPRESS(field) state.field = field ? 255 : 0
    CControllerState GetDecompressed()
    {
        CControllerState state{};
        state.LeftStickX = LeftStickX;
        state.LeftStickY = LeftStickY;

        DECOMPRESS(LeftShoulder1);
        DECOMPRESS(LeftShoulder2);
        DECOMPRESS(RightShoulder1);
        DECOMPRESS(RightShoulder2);
        DECOMPRESS(DPadUp);
        DECOMPRESS(DPadDown);
        DECOMPRESS(DPadLeft);
        DECOMPRESS(DPadRight);
        DECOMPRESS(Start);
        DECOMPRESS(Select);
        DECOMPRESS(ButtonSquare);
        DECOMPRESS(ButtonTriangle);
        DECOMPRESS(ButtonCross);
        DECOMPRESS(ButtonCircle);
        DECOMPRESS(ShockButtonL);
        DECOMPRESS(ShockButtonR);
        DECOMPRESS(m_bChatIndicated);
        DECOMPRESS(m_bPedWalk);
        DECOMPRESS(m_bVehicleMouseLook);
        DECOMPRESS(m_bRadioTrackSkip);
        return state;
    }
#undef DECOMPRESS
};

#pragma pack(1)
struct CSyncAutoPilot
{
public:
    CNodeAddress        m_currentAddress;
    CNodeAddress        m_startingRouteNode;
    CNodeAddress        m_endingRouteNode;
    unsigned int        m_nSpeedScaleFactor;
    CCarPathLinkAddress m_nCurrentPathNodeInfo;
    CCarPathLinkAddress m_nNextPathNodeInfo;
    CCarPathLinkAddress m_nPreviousPathNodeInfo;
    unsigned char       m_nCurrentLane;
    unsigned char       m_nNextLane;
    eCarDrivingStyle    m_nCarDrivingStyle;
    eCarMission         m_nCarMission;
    char                m_nTempAction;
    unsigned int        m_nTempActionTime;
    unsigned char       m_ucTempActionMode;
    unsigned char       m_ucCarMissionModeCounter;
    float               m_speed;
    float               m_fMaxTrafficSpeed;
    unsigned char       m_nCruiseSpeed;
    unsigned char       m_ucHeliSpeedMult;
    unsigned char       m_nCarCtrlFlags;
    unsigned char       m_nMovementFlags;
    unsigned char       m_nStraightLineDistance;
    CVector             m_vecDestinationCoors;
    CNodeAddress        m_aPathFindNodesInfo[8];
    unsigned short      m_nPathFindNodesCount;

    CSyncAutoPilot()
    {

    }

    CSyncAutoPilot(const CAutoPilot& autoPilot)
    {
        m_currentAddress =              autoPilot.m_currentAddress;
        m_startingRouteNode =           autoPilot.m_startingRouteNode;
        m_endingRouteNode =             autoPilot.field_8;
        m_nSpeedScaleFactor =           autoPilot.m_nSpeedScaleFactor;
        m_nCurrentPathNodeInfo =        autoPilot.m_nCurrentPathNodeInfo;
        m_nNextPathNodeInfo =           autoPilot.m_nNextPathNodeInfo;
        m_nPreviousPathNodeInfo =       autoPilot.m_nPreviousPathNodeInfo;
        m_nCurrentLane =                autoPilot.m_nCurrentLane;
        m_nNextLane =                   autoPilot.m_nNextLane;
        m_nCarDrivingStyle =            autoPilot.m_nCarDrivingStyle;
        m_nCarMission =                 autoPilot.m_nCarMission;
        m_nTempAction =                 autoPilot.m_nTempAction;
        m_nTempActionTime =             autoPilot.m_nTempActionTime - CTimer::m_snTimeInMilliseconds;
        m_ucTempActionMode =            autoPilot.field_34;
        m_ucCarMissionModeCounter =     autoPilot.field_35;
        m_speed =                       autoPilot.field_38;
        m_fMaxTrafficSpeed =            autoPilot.m_fMaxTrafficSpeed;
        m_nCruiseSpeed =                autoPilot.m_nCruiseSpeed;
        m_ucHeliSpeedMult =             autoPilot.field_48[0];
        m_nCarCtrlFlags =               autoPilot.m_nCarCtrlFlags;
        m_nMovementFlags =              autoPilot.field_4C;
        m_nStraightLineDistance =       autoPilot.m_nStraightLineDistance;
        m_vecDestinationCoors =         autoPilot.m_vecDestinationCoors;
        memcpy(m_aPathFindNodesInfo, autoPilot.m_aPathFindNodesInfo, sizeof(m_aPathFindNodesInfo));
        m_nPathFindNodesCount =         autoPilot.m_nPathFindNodesCount;
    }

    void WriteTo(CAutoPilot& autoPilot)
    {
        autoPilot.m_currentAddress =            m_currentAddress;
        autoPilot.m_startingRouteNode =         m_startingRouteNode;
        autoPilot.field_8 =                     m_endingRouteNode;
        autoPilot.m_nSpeedScaleFactor =         m_nSpeedScaleFactor;
        autoPilot.m_nCurrentPathNodeInfo =      m_nCurrentPathNodeInfo;
        autoPilot.m_nNextPathNodeInfo =         m_nNextPathNodeInfo;
        autoPilot.m_nPreviousPathNodeInfo =     m_nPreviousPathNodeInfo;
        autoPilot.m_nCurrentLane =              m_nCurrentLane;
        autoPilot.m_nNextLane =                 m_nNextLane;
        autoPilot.m_nCarDrivingStyle =          m_nCarDrivingStyle;
        autoPilot.m_nCarMission =               m_nCarMission;
        autoPilot.m_nTempAction =               m_nTempAction;
        autoPilot.m_nTempActionTime =           m_nTempActionTime + CTimer::m_snTimeInMilliseconds;
        autoPilot.field_34 =                    m_ucTempActionMode;
        autoPilot.field_35 =                    m_ucCarMissionModeCounter;
        autoPilot.field_38 =                    m_speed;
        autoPilot.m_fMaxTrafficSpeed =          m_fMaxTrafficSpeed;
        autoPilot.m_nCruiseSpeed =              m_nCruiseSpeed;
        autoPilot.field_48[0] =                 m_ucHeliSpeedMult;
        autoPilot.m_nCarCtrlFlags =             m_nCarCtrlFlags;
        autoPilot.field_4C =                    m_nMovementFlags;
        autoPilot.m_nStraightLineDistance =     m_nStraightLineDistance;
        autoPilot.m_vecDestinationCoors =       m_vecDestinationCoors;
        memcpy(autoPilot.m_aPathFindNodesInfo,  m_aPathFindNodesInfo, sizeof(m_aPathFindNodesInfo));
        autoPilot.m_nPathFindNodesCount =       m_nPathFindNodesCount;
    }
};