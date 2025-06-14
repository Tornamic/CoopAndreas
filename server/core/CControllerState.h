#if !defined(_COOPSERVER_INPUTCONTROLLERS_CONTROLLERSTATE_H_) || !defined(_COOPSERVER_CCONTROLLERSTATE_H_)
#define _COOPSERVER_INPUTCONTROLLERS_CONTROLLERSTATE_H_
#define _COOPSERVER_CCONTROLLERSTATE_H_

#include <cstdint>

//#pragma pack(1)
struct CCompressedControllerState
{
    int16_t LeftStickX; // move/steer left (-128?)/right (+128)
    int16_t LeftStickY; // move back(+128)/forwards(-128?)

    union
    {
        struct
        {
            uint32_t LeftShoulder1 : 1;
            uint32_t LeftShoulder2 : 1;
            uint32_t RightShoulder1 : 1; // target / hand brake
            uint32_t RightShoulder2 : 1;
            uint32_t DPadUp : 1; // radio change up
            uint32_t DPadDown : 1; // radio change down
            uint32_t DPadLeft : 1; // Skip trip/Action / Negative talk reply
            uint32_t DPadRight : 1; // Next user MP3 track / Positive talk reply

            uint32_t Start : 1; // Pause
            uint32_t Select : 1; // Camera modes
            uint32_t ButtonSquare : 1; // jump / reverse
            uint32_t ButtonTriangle : 1; // get in/out
            uint32_t ButtonCross : 1; // sprint / accelerate
            uint32_t ButtonCircle : 1; // fire
            uint32_t ShockButtonL : 1;
            uint32_t ShockButtonR : 1; // look behind

            uint32_t m_bChatIndicated : 1;
            uint32_t m_bPedWalk : 1;
            uint32_t m_bVehicleMouseLook : 1;
            uint32_t m_bRadioTrackSkip : 1;
        };
        uint32_t compressed; // To access all bits as a single value if needed
    };

    union
    {
        struct
        {
            uint16_t bCamera : 1;
            uint16_t unk2 : 1;
            uint16_t bPlayerAwaitsInGarage : 1;
            uint16_t bPlayerOnInteriorTransition : 1;
            uint16_t unk3 : 1;
            uint16_t bPlayerSafe : 1;
            uint16_t bPlayerTalksOnPhone : 1;
            uint16_t bPlayerSafeForCutscene : 1;

            uint16_t bPlayerSkipsToDestination : 1;
            uint16_t bDisablePlayerEnterCar : 1;
            uint16_t bDisablePlayerDuck : 1;
            uint16_t bDisablePlayerFireWeapon : 1;
            uint16_t bDisablePlayerFireWeaponWithL1 : 1;
            uint16_t bDisablePlayerCycleWeapon : 1;
            uint16_t bDisablePlayerJump : 1;
            uint16_t bDisablePlayerDisplayVitalStats : 1;
        };
        uint16_t disableFlags;
    };
};


class CControllerState 
{    
    public:
        CControllerState() { }
        short LeftStickX; // move/steer left (-128?)/right (+128)
        short LeftStickY; // move back(+128)/forwards(-128?)
        short RightStickX; // numpad 6(+128)/numpad 4(-128?)
        short RightStickY;
    
        short LeftShoulder1;
        short LeftShoulder2;
        short RightShoulder1; // target / hand brake
        short RightShoulder2;
    
        short DPadUp; // radio change up           Next radio station / Call gang forward/Recruit gang member
        short DPadDown; // radio change down       Previous radio station / Gang stay back/Release gang (hold)
        short DPadLeft; //                         Skip trip/Action / Negative talk reply
        short DPadRight; //                        Next user MP3 track / Positive talk reply
    
        short Start;                             //Pause
        short Select;                            //Camera modes
    
        short ButtonSquare; // jump / reverse      Break/Reverse / Jump/Climb
        short ButtonTriangle; // get in/out        Exit vehicle / Enter veihcle
        short ButtonCross; // sprint / accelerate  Accelerate / Sprint/Swim
        short ButtonCircle; // fire                Fire weapon
    
        short ShockButtonL;
        short ShockButtonR; // look behind
    
        short m_bChatIndicated;
        short m_bPedWalk;
        short m_bVehicleMouseLook;
        short m_bRadioTrackSkip;
        ~CControllerState() { }
};

#endif