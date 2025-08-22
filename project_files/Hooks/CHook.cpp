#include "stdafx.h"
#include "PlayerHooks.h"
#include "VehicleHooks.h"
#include "TaskHooks.h"
#include "WorldHooks.h"
#include "PedHooks.h"
#include "GameHooks.h"
#include "StatsHooks.h"
#include "CrashfixHooks.h"
#include "MissionAudioHooks.h"
#include "Commands/CommandHooks.h"
#include "SaveLoadHooks.h"
#include "RadarHooks.h"
#include "ProjectileHooks.h"

void CHook::Init()
{
    VehicleHooks::InjectHooks();
    PlayerHooks::InjectHooks();
    TaskHooks::InjectHooks();
    WorldHooks::InjectHooks();
    PedHooks::InjectHooks();
    GameHooks::InjectHooks();
    StatsHooks::InjectHooks();
    CommandHooks::InjectHooks();
    CrashfixHooks::InjectHooks();
    MissionAudioHooks::InjectHooks();
    SaveLoadHooks::InjectHooks();
    RadarHooks::InjectHooks();
    ProjectileHooks::InjectHooks();
}