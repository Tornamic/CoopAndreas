/*
    Plugin-SDK (Grand Theft Auto San Andreas) header file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "PluginBase.h"

class CAutomobile;

enum PLUGIN_API eDamageState
{
    DAMSTATE_OK = 0,
    DAMSTATE_OPENED = 1,
    DAMSTATE_DAMAGED = 2,
    DAMSTATE_OPENED_DAMAGED = 3,
    DAMSTATE_NOTPRESENT = 4
};

enum PLUGIN_API tComponent
{
    COMPONENT_WHEEL_LF = 1,
    COMPONENT_WHEEL_RF = 2,
    COMPONENT_WHEEL_LR = 3,
    COMPONENT_WHEEL_RR = 4,
    COMPONENT_BONNET = 5,
    COMPONENT_BOOT = 6,
    COMPONENT_DOOR_LF = 7,
    COMPONENT_DOOR_RF = 8,
    COMPONENT_DOOR_LR = 9,
    COMPONENT_DOOR_RR = 10,
    COMPONENT_WING_LF = 11,
    COMPONENT_WING_RF = 12,
    COMPONENT_WING_LR = 13,
    COMPONENT_WING_RR = 14,
    COMPONENT_WINDSCREEN = 15,
    COMPONENT_BUMP_FRONT = 16,
    COMPONENT_BUMP_REAR = 17
};

enum PLUGIN_API tComponentGroup
{
    COMPGROUP_PANEL = 0,
    COMPGROUP_WHEEL = 1,
    COMPGROUP_DOOR = 2,
    COMPGROUP_BONNET = 3,
    COMPGROUP_BOOT = 4,
    COMPGROUP_LIGHT = 5,
    COMPGROUP_NA = 6
};

enum PLUGIN_API eWheels
{
    WHEEL_FRONT_LEFT = 0,
    WHEEL_REAR_LEFT = 1,
    WHEEL_FRONT_RIGHT = 2,
    WHEEL_REAR_RIGHT = 3
};

enum PLUGIN_API ePanels
{
    WING_FRONT_LEFT = 0,
    WING_FRONT_RIGHT = 1,
    WINDSCREEN = 4,
    BUMP_FRONT = 5,
    BUMP_REAR = 6
};

enum PLUGIN_API eDoors
{
    BONNET = 0,
    BOOT = 1,
    DOOR_FRONT_LEFT = 2,
    DOOR_FRONT_RIGHT = 3,
    DOOR_REAR_LEFT = 4,
    DOOR_REAR_RIGHT = 5
};

enum PLUGIN_API eLights
{
    LIGHT_FRONT_LEFT = 0,
    LIGHT_FRONT_RIGHT = 1,
    LIGHT_REAR_RIGHT = 2,
    LIGHT_REAR_LEFT = 3
};

enum eWheelDamageState
{
    DT_WHEEL_INTACT = 0,
    DT_WHEEL_BURST,
    DT_WHEEL_MISSING
};

enum eDoorDamageState
{
    DT_DOOR_INTACT = 0,
    DT_DOOR_SWINGING_FREE,
    DT_DOOR_BASHED,
    DT_DOOR_BASHED_AND_SWINGING_FREE,
    DT_DOOR_MISSING
};

enum eLightDamageState
{
    DT_LIGHT_OK = 0,
    DT_LIGHT_SMASHED
};

enum ePanelDamageState
{
    DT_PANEL_INTACT = 0,
    DT_PANEL_BASHED,
    DT_PANEL_BASHED2,
    DT_PANEL_MISSING
};

enum ePlaneDamageState
{
    DT_PLANE_INTACT = 0,
    DT_PLANE_BASHED,
    DT_PLANE_MISSING
};

enum eEngineDamageState : uint8_t
{
    DT_ENGINE_0 = 0,      // 0
    DT_ENGINE_100 = 100,  // 1
    DT_ENGINE_200 = 200,  // 2
    DT_ENGINE_215 = 215,  // 3
    DT_ENGINE_225 = 225,  // 4
    DT_ENGINE_250 = 250  // 5
};

extern inline uint8_t EngineDamageStateGetCompressed(eEngineDamageState s)
{
    if (s > DT_ENGINE_250)
    {
        s = DT_ENGINE_250;
    }

    switch (s)
    {
        default:
        case DT_ENGINE_0:
            return 0;
        case DT_ENGINE_100:
            return 1;
        case DT_ENGINE_200:
            return 2;
        case DT_ENGINE_215:
            return 3;
        case DT_ENGINE_225:
            return 4;
        case DT_ENGINE_250:
            return 5;
    }
}

extern inline eEngineDamageState EngineDamageStateGetDecompressed(uint8_t s)
{
    switch (s)
    {
        default:
        case 0:
            return DT_ENGINE_0;
        case 1:
            return DT_ENGINE_100;
        case 2:
            return DT_ENGINE_200;
        case 3:
            return DT_ENGINE_215;
        case 4:
            return DT_ENGINE_225;
        case 5:
            return DT_ENGINE_250;
    }
}

extern inline const char* WheelStateToString(eWheelDamageState s)
{
    switch (s)
    {
        case DT_WHEEL_INTACT:
            return "Intact";
        case DT_WHEEL_BURST:
            return "Burst";
        case DT_WHEEL_MISSING:
            return "Missing";
        default:
            return "Unknown";
    }
}

extern inline const char* DoorStateToString(eDoorDamageState s)
{
    switch (s)
    {
        case DT_DOOR_INTACT:
            return "Intact";
        case DT_DOOR_SWINGING_FREE:
            return "Swinging";
        case DT_DOOR_BASHED:
            return "Bashed";
        case DT_DOOR_BASHED_AND_SWINGING_FREE:
            return "Bashed+Swinging";
        case DT_DOOR_MISSING:
            return "Missing";
        default:
            return "Unknown";
    }
}

extern inline const char* LightStateToString(eLightDamageState s)
{
    return s == DT_LIGHT_OK ? "OK" : s == DT_LIGHT_SMASHED ? "Smashed" : "Unknown";
}

extern inline const char* PanelStateToString(ePanelDamageState s)
{
    switch (s)
    {
        case DT_PANEL_INTACT:
            return "Intact";
        case DT_PANEL_BASHED:
            return "Bashed";
        case DT_PANEL_BASHED2:
            return "Bashed2";
        case DT_PANEL_MISSING:
            return "Missing";
        default:
            return "Unknown";
    }
}

class PLUGIN_API CDamageManager
{
public:
    float m_fWheelDamageEffect = 0.0f;
    unsigned char m_nEngineStatus = 0;
    union
    {
        unsigned char m_anWheelsStatus[4] = {0};
        struct
        {
            unsigned char m_nRightRearWheelsStatus;
            unsigned char m_nRightFrontWheelsStatus;
            unsigned char m_nLeftRearWheelsStatus;
            unsigned char m_nLeftFrontWheelsStatus;
        };
    };
    union
    {
        unsigned char m_anDoorsStatus[6] = { 0 };
        struct
        {
            unsigned char m_nBonnetStatus;
            unsigned char m_nBootStatus;
            unsigned char m_nLeftFrontDoorStatus;
            unsigned char m_nRightFrontDoorStatus;
            unsigned char m_nLeftRearDoorStatus;
            unsigned char m_nRightRearDoorStatus;
        };
    };
    union
    {
        unsigned int m_nLightsStatus = 0;
        struct
        {
            unsigned int m_nLeftFrontLightStatus : 2;
            unsigned int m_nRightFrontLightStatus : 2;
            unsigned int m_nRightRearLightStatus : 2;
            unsigned int m_nLeftRearLightStatus : 2;
        };
    };
    union
    {
        unsigned int m_nPanelsStatus = 0;

        struct
        {
            unsigned int m_nLeftFrontWingStatus : 2;
            unsigned int m_nRightFrontWingStatus : 2;
            unsigned int m_nLeftRearWingStatus : 2;
            unsigned int m_nRightRearWingStatus : 2;
            unsigned int m_nWindscreenStatus : 2;
            unsigned int m_nFrontBumperStatus : 2;
            unsigned int m_nRearBumperStatus : 2;
        };
    };

    bool operator==(const CDamageManager& r)
    {
        return memcmp(this, &r, sizeof(*this)) == 0;
    }
    bool operator!=(const CDamageManager& r)
    {
        return !(*this == r);
    }

    // funcs

    template <typename Stream>
    bool Serialize(Stream& stream)
    {
        if (Stream::IsReading)
        {
            m_fWheelDamageEffect = 0.5f; // always 0.5
        }

        if(Stream::IsWriting)
        {
            uint8_t temp = EngineDamageStateGetCompressed((eEngineDamageState)m_nEngineStatus);
            serialize_int(stream, temp, 0, 5);
        }
        else if (Stream::IsReading)
        {
            uint8_t temp;
            serialize_int(stream, temp, 0, 5);
            m_nEngineStatus = EngineDamageStateGetDecompressed(temp);
        }

        for (uint8_t i = 0; i < 4; i++)
        {
            serialize_int(stream, m_anWheelsStatus[i], DT_WHEEL_INTACT, DT_WHEEL_MISSING);
        }
        
        for (uint8_t i = 0; i < 6; i++)
        {
            serialize_int(stream, m_anDoorsStatus[i], DT_DOOR_INTACT, DT_DOOR_MISSING);
        }

        serialize_int(stream, m_nLeftFrontLightStatus, DT_LIGHT_OK, DT_LIGHT_SMASHED);
        serialize_int(stream, m_nRightFrontLightStatus, DT_LIGHT_OK, DT_LIGHT_SMASHED);
        serialize_int(stream, m_nRightRearLightStatus, DT_LIGHT_OK, DT_LIGHT_SMASHED);
        serialize_int(stream, m_nLeftRearLightStatus, DT_LIGHT_OK, DT_LIGHT_SMASHED);

        serialize_int(stream, m_nLeftFrontWingStatus, DT_PANEL_INTACT, DT_PANEL_MISSING);
        serialize_int(stream, m_nRightFrontWingStatus, DT_PANEL_INTACT, DT_PANEL_MISSING);
        serialize_int(stream, m_nLeftRearWingStatus, DT_PANEL_INTACT, DT_PANEL_MISSING);
        serialize_int(stream, m_nRightRearWingStatus, DT_PANEL_INTACT, DT_PANEL_MISSING);
        serialize_int(stream, m_nWindscreenStatus, DT_PANEL_INTACT, DT_PANEL_MISSING);
        serialize_int(stream, m_nFrontBumperStatus, DT_PANEL_INTACT, DT_PANEL_MISSING);
        serialize_int(stream, m_nRearBumperStatus, DT_PANEL_INTACT, DT_PANEL_MISSING);

        return true;
    }

    // damageCompId - eLights/ePanel/... id for this component
    bool GetComponentGroup(tComponent component, tComponentGroup* group, unsigned char* damageCompId);
    void ResetDamageStatus();
    void SetLightStatus(eLights light, unsigned int status);
    unsigned int GetLightStatus(eLights light);
    void SetPanelStatus(int panel, unsigned int status);
    unsigned int GetPanelStatus(int arg0);
    void SetWheelStatus(int wheel, unsigned int status);
    unsigned int GetWheelStatus(int wheel);
    void SetDoorStatus(eDoors door, unsigned int status);
    void SetDoorStatus(int doorNodeIndex, unsigned int status);
    unsigned int GetDoorStatus(int doorNodeIndex);
    unsigned int GetDoorStatus(eDoors door);
    // Status is a value between 0-250
    void SetEngineStatus(unsigned int status);
    // Status is a value between 0-250
    unsigned int GetEngineStatus();
    void SetAeroplaneCompStatus(int component, unsigned int status);
    unsigned int GetAeroplaneCompStatus(int component);
    // damageCompId - eLights/ePanel/... id for this component
    bool ProgressDoorDamage(unsigned char damageCompId, CAutomobile* car);
    // Empty function
    bool ProgressEngineDamage(float arg0);
    // Set next level of damage to panel
    bool ProgressPanelDamage(unsigned char panel);
    // Set next level of damage to panel
    bool ProgressWheelDamage(unsigned char wheel);
    // Set next level of damage to aeroplane component
    bool ProgressAeroplaneDamage(unsigned char component);
    bool ApplyDamage(CAutomobile* car, tComponent component, float intensity, float arg3);
    void FuckCarCompletely(bool skipWheels);
    // returns -1 if no node for this panel
    int GetCarNodeIndexFromPanel(ePanels panel);
    // returns -1 if no node for this door
    int GetCarNodeIndexFromDoor(eDoors door);
    void Reset();
};

VALIDATE_SIZE(CDamageManager, 0x18);
