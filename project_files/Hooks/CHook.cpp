#include "../stdafx.h"
#include "CHook.h"
#include "PlayerHooks.h"
#include "VehicleHooks.h"
#include "TaskHooks.h"
#include "WorldHooks.h"
#include "PedHooks.h"
#include "GameHooks.h"

void CHook::Init()
{
    VehicleHooks::InjectHooks();
    PlayerHooks::InjectHooks();
    TaskHooks::InjectHooks();
    WorldHooks::InjectHooks();
    PedHooks::InjectHooks();
    GameHooks::InjectHooks();
}