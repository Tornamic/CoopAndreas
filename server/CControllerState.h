#pragma once
class CControllerState {
public:
    signed short LeftStickX; // move/steer left (-128?)/right (+128)
    signed short LeftStickY; // move back(+128)/forwards(-128?)
    signed short RightStickX; // numpad 6(+128)/numpad 4(-128?)
    signed short RightStickY;

    signed short LeftShoulder1;
    signed short LeftShoulder2;
    signed short RightShoulder1; // target / hand brake
    signed short RightShoulder2;

    signed short DPadUp; // radio change up           Next radio station / Call gang forward/Recruit gang member
    signed short DPadDown; // radio change down       Previous radio station / Gang stay back/Release gang (hold)
    signed short DPadLeft; //                         Skip trip/Action / Negative talk reply
    signed short DPadRight; //                        Next user MP3 track / Positive talk reply

    signed short Start;                             //Pause
    signed short Select;                            //Camera modes

    signed short ButtonSquare; // jump / reverse      Break/Reverse / Jump/Climb
    signed short ButtonTriangle; // get in/out        Exit vehicle / Enter veihcle
    signed short ButtonCross; // sprint / accelerate  Accelerate / Sprint/Swim
    signed short ButtonCircle; // fire                Fire weapon

    signed short ShockButtonL;
    signed short ShockButtonR; // look behind

    signed short m_bChatIndicated;
    signed short m_bPedWalk;
    signed short m_bVehicleMouseLook;
    signed short m_bRadioTrackSkip;
};

#pragma pack(1)
struct CCompressedControllerState
{
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
};