// Server stand-in for plugin-sdk CRadar.h: only what the shared packets need.
// Keep in sync with third_party/plugin-sdk/plugin_sa/game_sa/CRadar.h
#pragma once

#define MAX_RADAR_TRACES (175)

enum eBlipType
{
    BLIP_NONE,
    BLIP_CAR,
    BLIP_CHAR,
    BLIP_OBJECT,
    BLIP_COORD,
    BLIP_CONTACTPOINT,
    BLIP_SPOTLIGHT,
    BLIP_PICKUP,
    BLIP_AIRSTRIP
};

enum eCoordBlipAppearance
{
    COORD_BLIP_APPEARANCE_NORMAL,
    COORD_BLIP_APPEARANCE_FRIEND,
    COORD_BLIP_APPEARANCE_ENEMY
};

enum eBlipDisplay
{
    BLIP_DISPLAY_NEITHER,      // 0
    BLIP_DISPLAY_MARKER_ONLY,  // 1
    BLIP_DISPLAY_BLIP_ONLY,    // 2
    BLIP_DISPLAY_BOTH          // 3
};

enum eBlipColour
{
    BLIP_COLOUR_RED,         // 0
    BLIP_COLOUR_GREEN,       // 1
    BLIP_COLOUR_BLUE,        // 2
    BLIP_COLOUR_WHITE,       // 3
    BLIP_COLOUR_YELLOW,      // 4
    BLIP_COLOUR_REDCOPY,     // 5 - What? It was BLIP_COLOUR_PURPLE.
    BLIP_COLOUR_BLUECOPY,    // 6 - Why? It was BLIP_COLOUR_CYAN.
    BLIP_COLOUR_THREAT,      // 7 - If BLIP_FLAG_FRIENDLY is not set (by default) it is BLIP_COLOUR_RED, else
                             // BLIP_COLOUR_BLUE.
    BLIP_COLOUR_DESTINATION  // 8 - Default color.
};

enum eRadarSprite {
    RADAR_SPRITE_NONE,          // 0
    RADAR_SPRITE_WHITE,         // 1
    RADAR_SPRITE_CENTRE,        // 2
    RADAR_SPRITE_MAP_HERE,      // 3
    RADAR_SPRITE_NORTH,         // 4
    RADAR_SPRITE_AIRYARD,       // 5
    RADAR_SPRITE_AMMUGUN,       // 6
    RADAR_SPRITE_BARBERS,       // 7
    RADAR_SPRITE_BIGSMOKE,      // 8
    RADAR_SPRITE_BOATYARD,      // 9
    RADAR_SPRITE_BURGERSHOT,    // 10
    RADAR_SPRITE_BULLDOZER,     // 11
    RADAR_SPRITE_CATALINAPINK,  // 12
    RADAR_SPRITE_CESARVIAPANDO, // 13 - What? R* mistype?
    RADAR_SPRITE_CHICKEN,       // 14
    RADAR_SPRITE_CJ,            // 15
    RADAR_SPRITE_CRASH1,        // 16
    RADAR_SPRITE_DINER,         // 17 - Yet another one?
    RADAR_SPRITE_EMMETGUN,      // 18
    RADAR_SPRITE_ENEMYATTACK,   // 19
    RADAR_SPRITE_FIRE,          // 20
    RADAR_SPRITE_GIRLFRIEND,    // 21
    RADAR_SPRITE_HOSTPITAL,     // 22 - Again?
    RADAR_SPRITE_LOGOSYNDICATE, // 23
    RADAR_SPRITE_MADDOG,        // 24
    RADAR_SPRITE_MAFIACASINO,   // 25
    RADAR_SPRITE_MCSTRAP,       // 26
    RADAR_SPRITE_MODGARAGE,     // 27
    RADAR_SPRITE_OGLOC,         // 28
    RADAR_SPRITE_PIZZA,         // 29
    RADAR_SPRITE_POLICE,        // 30
    RADAR_SPRITE_PROPERTYG,     // 31
    RADAR_SPRITE_PROPERTYR,     // 32
    RADAR_SPRITE_RACE,          // 33
    RADAR_SPRITE_RYDER,         // 34
    RADAR_SPRITE_SAVEGAME,      // 35
    RADAR_SPRITE_SCHOOL,        // 36
    RADAR_SPRITE_QMARK,         // 37
    RADAR_SPRITE_SWEET,         // 38
    RADAR_SPRITE_TATTOO,        // 39
    RADAR_SPRITE_THETRUTH,      // 40
    RADAR_SPRITE_WAYPOINT,      // 41
    RADAR_SPRITE_TORENORANCH,   // 42
    RADAR_SPRITE_TRIADS,        // 43
    RADAR_SPRITE_TRIADSCASINO,  // 44
    RADAR_SPRITE_TSHIRT,        // 45
    RADAR_SPRITE_WOOZIE,        // 46
    RADAR_SPRITE_ZERO,          // 47
    RADAR_SPRITE_DATEDISCO,     // 48
    RADAR_SPRITE_DATEDRINK,     // 49
    RADAR_SPRITE_DATEFOOD,      // 50
    RADAR_SPRITE_TRUCK,         // 51
    RADAR_SPRITE_CASH,          // 52
    RADAR_SPRITE_FLAG,          // 53
    RADAR_SPRITE_GYM,           // 54
    RADAR_SPRITE_IMPOUND,       // 55
    RADAR_SPRITE_LIGHT,         // 56
    RADAR_SPRITE_RUNWAY,        // 57
    RADAR_SPRITE_GANGB,         // 58
    RADAR_SPRITE_GANGP,         // 59
    RADAR_SPRITE_GANGY,         // 60
    RADAR_SPRITE_GANGN,         // 61
    RADAR_SPRITE_GANGG,         // 62
    RADAR_SPRITE_SPRAY,/*,       // 63
    RADAR_SPRITE_TORENO*/       // 64
    RADAR_SPRITE_COUNT,
};
