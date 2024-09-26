#pragma once
#include "stdafx.h"

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
        };
        unsigned int compressed; // To access all bits as a single value if needed
    };

    ÑCompressedControllerState()
    {
        LeftStickX = LeftStickY = compressed = 0;
    }

#define COMPRESS(field) field = (state.field > 0)
    ÑCompressedControllerState(const CControllerState& state)
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