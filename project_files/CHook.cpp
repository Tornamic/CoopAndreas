#include "stdafx.h"

bool pPressingDuck[MAX_SERVER_PLAYERS + 2] = { false };

void __fastcall CPlayerPed__ProcessControl_Hook(CPlayerPed* This)
{
    CPlayerPed* localPlayer = FindPlayerPed(0);

    if (This == localPlayer)
    {
        patch::SetRaw(0x6884C4, "\xD9\x96\x5C\x05\x00\x00", 6);
        plugin::CallMethod<0x60EA90, CPlayerPed*>(This);
        patch::Nop(0x6884C4, 6);
        return;
    }

    CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(This);

    if (player == nullptr)
        return;

    CWorld::PlayerInFocus = player->GetInternalId();

    CPad* pad = This->GetPadFromPlayer();

    CControllerState newOldState = pad->NewState;
    CControllerState oldOldState = pad->OldState;

    CUtil::CopyControllerState(pad->OldState, player->m_oOnFoot->controllerState);
    CUtil::CopyControllerState(pad->NewState, player->m_lOnFoot->controllerState);

    if (CUtil::IsDucked(player->m_pPed) != player->m_lOnFoot->ducking) //Forcing crouch sync
    {
        pPressingDuck[CWorld::PlayerInFocus] = true;
        player->m_oShockButtonL = 0;
        player->m_lShockButtonL = 255;
    }
    else if (pPressingDuck[CWorld::PlayerInFocus] == true)
    {
        pPressingDuck[CWorld::PlayerInFocus] = false;
        player->m_oShockButtonL = 255;
        player->m_lShockButtonL = 0;
    }

    pad->OldState.ShockButtonL = player->m_oShockButtonL;
    pad->NewState.ShockButtonL = player->m_lShockButtonL;

    player->m_pPed->m_fHealth = player->m_lOnFoot->health;
    player->m_pPed->m_fArmour = player->m_lOnFoot->armour;

    player->m_pPed->m_vecMoveSpeed = player->m_lOnFoot->velocity;

    player->m_pPed->m_fAimingRotation =
        player->m_pPed->m_fCurrentRotation = player->m_lOnFoot->rotation;

    plugin::CallMethod<0x60EA90, CPlayerPed*>(This);

    CWorld::PlayerInFocus = 0;

    pad->NewState = newOldState;
    pad->OldState = oldOldState;
}


void __fastcall CVehicle__ProcessControl_Hook()
{
    CVehicle* vehicle = nullptr;
    DWORD vtbl = 0;
    DWORD call_addr = 0;

    _asm mov vehicle, ecx
    _asm mov eax, [ecx]
        _asm mov vtbl, eax

    if (vtbl == 0x871120)      // CAutomobile
        call_addr = 0x6B1880;
    else if (vtbl == 0x8721A0) // CBoat
        call_addr = 0x6F1770;
    else if (vtbl == 0x871360) // CBike
        call_addr = 0x6B9250;
    else if (vtbl == 0x871948) // CPlane
        call_addr = 0x6C9260;
    else if (vtbl == 0x871680) // CHeli
        call_addr = 0x6C7050;
    else if (vtbl == 0x871528) // CBmx
        call_addr = 0x6BFA30;
    else if (vtbl == 0x8717d8) // CMonsterTruck
        call_addr = 0x6C8250;
    else if (vtbl == 0x871AE8) // CQuadBike
        call_addr = 0x6CDCC0;
    else if (vtbl == 0x872370) // CTrain
        call_addr = 0x6F86A0;


    CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(vehicle->m_pDriver);

    if (vehicle->m_pDriver == FindPlayerPed(0) || player == nullptr)
    {
        plugin::CallMethod<0x502280, CAEVehicleAudioEntity*>(&vehicle->m_vehicleAudio);
        _asm mov ecx, vehicle
        _asm mov eax, call_addr
        _asm call eax
        return;
    }


    CWorld::PlayerInFocus = player->GetInternalId();

    CPad* pad = player->m_pPed->GetPadFromPlayer();

    CControllerState newOldState = pad->NewState;
    CControllerState oldOldState = pad->OldState;

    CUtil::CopyControllerState(pad->OldState, player->m_oOnFoot->controllerState);
    CUtil::CopyControllerState(pad->NewState, player->m_lOnFoot->controllerState);

    pad->OldState.ShockButtonL = player->m_oOnFoot->controllerState.ShockButtonL;
    pad->NewState.ShockButtonL = player->m_lOnFoot->controllerState.ShockButtonL;

    player->m_pPed->m_fHealth = player->m_lOnFoot->health;
    player->m_pPed->m_fArmour = player->m_lOnFoot->armour;

    player->m_pPed->m_nPedType = PED_TYPE_CIVMALE;

    plugin::CallMethod<0x502280, CAEVehicleAudioEntity*>(&vehicle->m_vehicleAudio);

    player->m_pPed->m_nPedType = PED_TYPE_PLAYER1;


    bool savedLookingLeft = *(bool*)0xB6F1A4;
    bool savedLookingRight = *(bool*)0xB6F1A5;
    *(bool*)0xB6F1A4 = player->m_lOnFoot->controllerState.LeftShoulder2 > 0;
    *(bool*)0xB6F1A5 = player->m_lOnFoot->controllerState.RightShoulder2 > 0;

    _asm mov ecx, vehicle
    _asm mov eax, call_addr
    _asm call eax

    CWorld::PlayerInFocus = 0;

    pad->NewState = newOldState;
    pad->OldState = oldOldState;

    *(bool*)0xB6F1A4 = savedLookingLeft;
    *(bool*)0xB6F1A5 = savedLookingRight;
}

static void __fastcall CWeapon__DoBulletImpact_Hook(CWeapon* weapon, int padding, CEntity* owner, CEntity* victim, CVector* startPoint, CVector* endPoint, CColPoint* colPoint, int incrementalHit)
{
    if (owner == FindPlayerPed(0))
    {
        CPackets::PlayerBulletShot* packet = new CPackets::PlayerBulletShot;

        packet->targetid = -1;

        if (victim != nullptr)
        {
            switch (victim->m_nType)
            {
                case eEntityType::ENTITY_TYPE_PED: // ped or player
                {
                    if (auto playerTarget = CNetworkPlayerManager::GetPlayer(victim))
                        packet->targetid = playerTarget->m_iPlayerId;
                    break;
                }
                case eEntityType::ENTITY_TYPE_VEHICLE:
                {
                    if (auto vehicleTarget = CNetworkVehicleManager::GetVehicle(victim))
                        packet->targetid = vehicleTarget->m_nVehicleId;
                    break;
                }
            }
            packet->entityType = victim->m_nType;
        }

        packet->startPos = *startPoint;
        packet->endPos = *endPoint;
        packet->colPoint = *colPoint;
        packet->incrementalHit = incrementalHit;

        CNetwork::SendPacket(CPacketsID::PLAYER_BULLET_SHOT, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED);

        weapon->DoBulletImpact(owner, victim, startPoint, endPoint, colPoint, incrementalHit);

        return;
    }
}

static void __fastcall CPedIK__PointGunInDirection_Hook(CPedIK* This, int padding, float dirX, float dirY, char flag, float float1)
{
    if (This->m_pPed == FindPlayerPed(0))
    {
        // save last aim data for syncing
        CLocalPlayer::m_vecLastAimX = dirX;
        CLocalPlayer::m_vecLastAimY = dirY;

        This->PointGunInDirection(dirX, dirY, flag, float1);
        return;
    }

    CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(This->m_pPed);

    if (player == nullptr)
    {
        This->PointGunInDirection(dirX, dirY, flag, float1);
        return;
    }

    if (player->m_lOnFoot == nullptr)
        return;

    player->m_pPed->m_fAimingRotation =
        player->m_pPed->m_fCurrentRotation = player->m_lOnFoot->rotation;

    This->PointGunInDirection(player->m_lOnFoot->aimX, player->m_lOnFoot->aimY, flag, float1);
}

// place waypoint
static CdeclEvent	 <AddressList<0x5775D2, H_CALL>, PRIORITY_AFTER, ArgPick5N<eBlipType, 0, CVector, 1, eBlipColour, 2, eBlipDisplay, 3, char*, 4>, void(eBlipType, CVector, eBlipColour, eBlipDisplay, char*)> waypointPlaceEvent;
static void PlaceWaypointHook(eBlipType type, CVector posn, eBlipColour color, eBlipDisplay blipDisplay, char* scriptName)
{
    CPackets::PlayerPlaceWaypoint packet = { 0, true, posn };
    CNetwork::SendPacket(CPacketsID::PLAYER_PLACE_WAYPOINT, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
}

// hide waypoint
static void __fastcall CRadar__ClearBlip_Hook(int blipIndex, int padding)
{
    CPackets::PlayerPlaceWaypoint packet = { 0, false, CVector(0, 0, 0) };
    CNetwork::SendPacket(CPacketsID::PLAYER_PLACE_WAYPOINT, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    CRadar::ClearBlip(blipIndex);
}

static void __cdecl CExplosion__AddExplosion(CEntity* newVictim, CPed* newCreator, eExplosionType type, CVector2D pos, float z, int time, char usesSound, float cameraShake, char isVisible)
{
    /*if (!CLocalPlayer::m_bIsHost)
        return;*/

    plugin::Call<0x736A50, CEntity*, CPed*, int, CVector2D, float, int, char, float, char>(newVictim, newCreator, type, pos, z, time, usesSound, cameraShake, isVisible);

    CPackets::AddExplosion addExplosionPacket{};

    addExplosionPacket.type = (unsigned char)type;
    addExplosionPacket.pos = CVector(pos.x, pos.y, z);
    addExplosionPacket.time = time;
    addExplosionPacket.usesSound = usesSound;
    addExplosionPacket.cameraShake = cameraShake;
    addExplosionPacket.isVisible = isVisible;

    CNetwork::SendPacket(CPacketsID::ADD_EXPLOSION, &addExplosionPacket, sizeof addExplosionPacket, ENET_PACKET_FLAG_RELIABLE);
}

static void __cdecl CWorld__Add_Hook(CEntity* entity)
{
    bool dontCreateEntity = false;

    if (entity->m_nType == eEntityType::ENTITY_TYPE_VEHICLE)
    {
        if (!CLocalPlayer::m_bIsHost)
            dontCreateEntity = true;
        else
        {
            CVehicle* vehicle = (CVehicle*)entity;
            CNetworkVehicle* networkVehicle = new CNetworkVehicle(vehicle);
            CNetworkVehicleManager::Add(networkVehicle);
        }

    }
    else if (entity->m_nType == eEntityType::ENTITY_TYPE_PED)
    {
        CPed* ped = (CPed*)entity;

        if (ped->m_nPedType > 1)
        {
            if (!CLocalPlayer::m_bIsHost)
                dontCreateEntity = true;
            else
            {
                CNetworkPed* networkPed = new CNetworkPed(ped);
                CNetworkPedManager::Add(networkPed);
            }
        }
    }

    if (dontCreateEntity)
    {
        //delete entity;
        return;
    }

    CWorld::Add(entity);
}
static void __cdecl CWorld__Remove_Hook(CEntity* entity)
{
    if (entity->m_nType == eEntityType::ENTITY_TYPE_VEHICLE)
    {
        CVehicle* vehicle = (CVehicle*)entity;
        CNetworkVehicle* networkVehicle = CNetworkVehicleManager::GetVehicle(vehicle);
        if (networkVehicle)
        {
            if (CLocalPlayer::m_bIsHost)
            {
                CNetworkVehicleManager::Remove(networkVehicle);
                delete networkVehicle;
            }
        }
    }
    else if (entity->m_nType == eEntityType::ENTITY_TYPE_PED)
    {
        CPed* ped = (CPed*)entity;
        if (ped->m_nPedType > 1)
        {
            CNetworkPed* networkPed = CNetworkPedManager::GetPed(ped);
            if (networkPed)
            {
                if (CLocalPlayer::m_bIsHost)
                {
                    CNetworkPedManager::Remove(networkPed);
                    delete networkPed;
                }
            }
        }
    }

    CWorld::Remove(entity);
}

// when local player enters any vehicle
static void __fastcall CTaskComplexEnterCarAsDriver__Ctor_Hook(CTaskComplexEnterCarAsDriver* This, int, CVehicle* vehicle)
{
    CNetworkVehicle* networkVehicle = CNetworkVehicleManager::GetVehicle(vehicle);

    if (networkVehicle == nullptr)
    {
        CChat::AddMessage("ERROR: this vehicle is not synced");
        plugin::CallMethod<0x6402F0, CTaskComplexEnterCarAsDriver*, CVehicle*>(This, vehicle);
        return;
    }

    CPackets::VehicleEnter packet{};

    packet.seatid = 0;
    packet.vehicleid = networkVehicle->m_nVehicleId;

    CNetwork::SendPacket(CPacketsID::VEHICLE_ENTER, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);

    plugin::CallMethod<0x6402F0, CTaskComplexEnterCarAsDriver*, CVehicle*>(This, vehicle);
}

static void __fastcall CTaskComplexLeaveCar__Ctor_Hook(CTaskComplexLeaveCar* This, int, CVehicle* vehicle, int targetDoor, int delayTime, bool sensibleLeaveCar, bool forceGetOut)
{
    CPackets::VehicleExit packet{};
    CNetwork::SendPacket(CPacketsID::VEHICLE_EXIT, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    plugin::CallMethod<0x63B8C0, CTaskComplexLeaveCar*, CVehicle*, int, int, bool, bool>(This, vehicle, targetDoor, delayTime, sensibleLeaveCar, forceGetOut);
}

static void __fastcall CPlayerPed__dctor_Hook(CPlayerPed* This, int)
{
    This->m_pPlayerData->m_nMeleeWeaponAnimReferencedExtra = 0;
    plugin::CallMethod<0x6093B0, CPlayerPed*>(This);
}

static void __fastcall CCarCtrl__RemoveDistantCars_Hook()
{
    if (CLocalPlayer::m_bIsHost)
        CCarCtrl::RemoveDistantCars();
}

static bool __fastcall CDamageManager__ApplyDamage_Hook(CDamageManager* This, int, CAutomobile* dm_comp, tComponent compId, float intensity, float a5)
{
    CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(dm_comp);
    if (vehicle != nullptr && CLocalPlayer::m_bIsHost)
    {
        CPackets::VehicleDamage packet{};
        packet.vehicleid = vehicle->m_nVehicleId;
        packet.damageManager = dm_comp->m_damageManager;
        CNetwork::SendPacket(CPacketsID::VEHICLE_DAMAGE, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    }
    return plugin::CallMethodAndReturn<bool, 0x6C24B0, CDamageManager*, CAutomobile*, tComponent, float, float>(This, dm_comp, compId, intensity, a5);
}

static void __fastcall CVehicle__SetRemap_Hook(CVehicle* This, int, int paintJobId)
{
    if (auto vehicle = CNetworkVehicleManager::GetVehicle(This))
        vehicle->m_nPaintJob = paintJobId;

    This->SetRemap(paintJobId);
}

static bool __cdecl CGameLogin__IsCoopGameGoingOn_Hook()
{
    return false;
}

static void __fastcall CVehicle__AddVehicleUpgrade_Hook(CVehicle* This, int, int modelid)
{
    if (auto vehicle = CNetworkVehicleManager::GetVehicle(This))
    {
        CPackets::VehicleComponentAdd packet{};
        packet.vehicleid = vehicle->m_nVehicleId;
        packet.componentid = modelid;
        CNetwork::SendPacket(CPacketsID::VEHICLE_COMPONENT_ADD, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    }
    This->AddVehicleUpgrade(modelid);
}

static void __fastcall CVehicle__RemoveVehicleUpgrade_Hook(CVehicle* This, int, int modelid)
{
    if (auto vehicle = CNetworkVehicleManager::GetVehicle(This))
    {
        CPackets::VehicleComponentRemove packet{};
        packet.vehicleid = vehicle->m_nVehicleId;
        packet.componentid = modelid;
        CNetwork::SendPacket(CPacketsID::VEHICLE_COMPONENT_REMOVE, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    }
    This->RemoveVehicleUpgrade(modelid);
}
CAutomobile* _automobile = nullptr;
CNetworkVehicle* _vehicle = nullptr;
static void __declspec(naked) CAutomobile__FireTruckControl_Hook()
{
    _asm mov _automobile, ecx
    _asm mov eax, [ecx]

        if (_automobile->m_pDriver == FindPlayerPed(0))
        {
            _automobile->FireTruckControl(0.0f);
        }
        else if (_vehicle = CNetworkVehicleManager::GetVehicle(_automobile))
        {
            _automobile->m_fDoomHorizontalRotation = _vehicle->m_fAimHorizontal;
            _automobile->m_fDoomVerticalRotation = _vehicle->m_fAimVertical;
            _automobile->FireTruckControl(0.0f);
            _automobile->m_fDoomHorizontalRotation = _vehicle->m_fAimHorizontal;
            _automobile->m_fDoomVerticalRotation = _vehicle->m_fAimVertical;
        }

    _asm retn 4
}

static void __fastcall CAutomobile__TankControl_Hook()
{
    CAutomobile* automobile = nullptr;
    _asm mov automobile, ecx
    _asm mov eax, [ecx]

        if (automobile->m_pDriver == FindPlayerPed(0))
        {
            automobile->TankControl();
            return;
        }

    CNetworkVehicle* vehicle = CNetworkVehicleManager::GetVehicle(automobile);
    if (vehicle != nullptr && automobile->m_pDriver != nullptr)
    {
        automobile->m_fDoomHorizontalRotation = vehicle->m_fAimHorizontal;
        automobile->m_fDoomVerticalRotation = vehicle->m_fAimVertical;
        automobile->TankControl();
        automobile->m_fDoomHorizontalRotation = vehicle->m_fAimHorizontal;
        automobile->m_fDoomVerticalRotation = vehicle->m_fAimVertical;
    }
}

static void __fastcall CTaskSimpleGangDriveBy__SetupStaticAnimForPlayer_Hook(CTaskSimpleGangDriveBy* This, int, CPed* ped)
{
    if (ped == FindPlayerPed(0) || !ped->IsPlayer())
    {
        plugin::CallMethod<0x621960, CTaskSimpleGangDriveBy*, CPed*>(This, ped);
        return;
    }

    CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(ped);

    CWorld::PlayerInFocus = player->GetInternalId();

    CVector savedAim = *(CVector*)0xB6F32C;
    eCamMode savedCameraMode = *(eCamMode*)0xB6F1A8;
    float savedAspectRatio = *(float*)0xC3EFA4;
    float savedExtZoom = *(float*)0xB6F250;

    *(CVector*)0xB6F32C = player->m_aPassengerAim;
    *(eCamMode*)0xB6F1A8 = eCamMode::MODE_AIMWEAPON_FROMCAR;
    *(float*)0xC3EFA4 = 0.0f;
    *(float*)0xB6F250 = 0.0f;

    plugin::CallMethod<0x621960, CTaskSimpleGangDriveBy*, CPed*>(This, ped);

    *(CVector*)0xB6F32C = savedAim;
    *(eCamMode*)0xB6F1A8 = savedCameraMode;
    *(float*)0xC3EFA4 = savedAspectRatio;
    *(float*)0xB6F250 = savedExtZoom;

    CWorld::PlayerInFocus = 0;
}

static void __fastcall CTaskSimpleGangDriveBy__ProcessAiming_Hook(CTaskSimpleGangDriveBy* This, int, CPed* ped)
{
    if (ped == FindPlayerPed(0) || !ped->IsPlayer())
    {
        plugin::CallMethod<0x628350, CTaskSimpleGangDriveBy*, CPed*>(This, ped);
        return;
    }

    ped->m_nPedType = ePedType::PED_TYPE_CIVMALE;
    plugin::CallMethod<0x628350, CTaskSimpleGangDriveBy*, CPed*>(This, ped);
    ped->m_nPedType = ePedType::PED_TYPE_PLAYER1;
}

static void __cdecl CMenuManager__DrawFrontEnd_FixChat_Hook(float alpha)
{
    CFont::SetAlphaFade(alpha);

    if (CChat::m_bInputActive)
        CChat::ShowInput(false);
}

static void __cdecl CPopulation__Update_Hook(bool generate)
{
    if (CLocalPlayer::m_bIsHost)
        CPopulation::Update(generate);
}


#pragma region WeatherTimeHooks

static void __cdecl CClock__RestoreClock_Hook()
{
    CClock::RestoreClock();
    CPacketHandler::GameWeatherTime__Trigger();
}

static void __cdecl CClock__SetGameClock_Hook(unsigned char h, unsigned char m, unsigned char d)
{
    CClock::SetGameClock(h, m, d);
    CPacketHandler::GameWeatherTime__Trigger();
}

DWORD ProcessCheat_Hook_Ret = 0x438589;
static void __declspec(naked) ProcessCheat_Hook()
{
    // finally figured out how to hook functions without masturbating RedirectCallRedirectCallRedirectCallRedirectCallRedirectCallRedirectCallRedirectCallRedirectCallRedirectCallRedirectCall
    __asm
    {
        call eax ; call orig code
        pop edi
        pop esi
        mov byte ptr ds:[0x969110], bl

        pushad ; store registers
    }

    // sync time and weather after processing cheat code
    CPacketHandler::GameWeatherTime__Trigger(); 

    __asm
    {
        popad ; restore registers

        jmp ProcessCheat_Hook_Ret ; jump to function continuation
    }
}

static void __cdecl CWeather__ForceWeather_Hook(short id)
{
    CWeather::ForceWeather(id);
    CPacketHandler::GameWeatherTime__Trigger();
}

static void __cdecl CWeather__ForceWeatherNow_Hook(short id)
{
    plugin::Call<0x402157>(id);
    CPacketHandler::GameWeatherTime__Trigger();
}

static void __cdecl CWeather__SetWeatherToAppropriateTypeNow_Hook()
{
    CWeather::SetWeatherToAppropriateTypeNow();
    CPacketHandler::GameWeatherTime__Trigger();
}
#pragma endregion

CTaskManager* pTaskMgr = nullptr;
CTask* pTask = nullptr;
int nTasksId = 0;
CNetworkPed* pNetworkPed = nullptr;
DWORD CTaskManager__SetTask_Hook_Ret = 0x681AF5;
static void __declspec(naked) CTaskManager__SetTask_Hook()
{
    __asm
    {

        // original code
        push ebx
        mov ebx, [esp+8]
        // -------------

        mov pTaskMgr, ecx // get *this

        pushad

        // get pTask
        mov eax, [esp+10h]
        mov pTask, eax
    }

    if (pTaskMgr && pTask)
    {
        pNetworkPed = CNetworkPedManager::GetPed(pTaskMgr->m_pPed);

        if (pNetworkPed)
        {
            CChat::AddMessage("SET PRIMARY %s", CDebugPedTasks::TaskNames[pTask->GetId()]);
        }
    }

    __asm
    {
        popad

        jmp CTaskManager__SetTask_Hook_Ret
    }
}

CTask* pTaskComplex = nullptr;
DWORD CTaskManager__SetTaskSecondary_Hook_Ret = 0x681B65;
static void __declspec(naked) CTaskManager__SetTaskSecondary_Hook()
{
    __asm
    {

        // original code
        push ebx
        mov ebx, [esp+8]
        // -------------

        mov pTaskMgr, ecx // get *this

        pushad

        // get pTask
        mov eax, [esp+10h]
        mov pTask, eax
    }

    if (pTaskMgr && pTask)
    {
        pNetworkPed = CNetworkPedManager::GetPed(pTaskMgr->m_pPed);

        if (pNetworkPed)
        {
            CChat::AddMessage("SET SECONDARY %s", CDebugPedTasks::TaskNames[pTask->GetId()]);
        }
    }

    __asm
    {
        popad

        jmp CTaskManager__SetTaskSecondary_Hook_Ret
    }
}


DWORD CTaskComplex__SetSubTask_Hook_Ret = 0x61A44E;
static void __declspec(naked) CTaskComplex__SetSubTask_Hook()
{
    __asm
    {
        // original code
        test edi, edi
        mov [esi + 8], edi
        // --------------

        pushad
        pushfd

        mov pTask, edi
    }

    if (pTask)
    {
        pNetworkPed = CNetworkPedManager::GetPed(pTaskMgr->m_pPed);

        if (pNetworkPed)
        {
            CChat::AddMessage("SET SUB %s", CDebugPedTasks::TaskNames[pTask->GetId()]);
        }
    }

    __asm
    {
        popfd
        popad

        jmp CTaskComplex__SetSubTask_Hook_Ret
    }
}

unsigned int nDeletingFlags = 0;
DWORD CTask__dtor_Hook_Ret = 0x61A667;
static void __declspec(naked) CTask__dtor_Hook()
{
    __asm
    {
        mov eax, ecx
        test [esp + 4], 1

        pushad
        pushfd
        
        mov pTask, ecx
    }

    CChat::AddMessage("DESTROY TASK %s", CDebugPedTasks::TaskNames[pTask->GetId()]);

    __asm
    {
        popfd
        popad

        jmp CTask__dtor_Hook_Ret
    }
}

void CHook::Init()
{
    patch::SetPointer(0x86D190, CPlayerPed__ProcessControl_Hook);

    patch::RedirectCall(0x73CD92, CWeapon__DoBulletImpact_Hook);
    patch::RedirectCall(0x741199, CWeapon__DoBulletImpact_Hook);
    patch::RedirectCall(0x7411DF, CWeapon__DoBulletImpact_Hook);
    patch::RedirectCall(0x7412DF, CWeapon__DoBulletImpact_Hook);
    patch::RedirectCall(0x741E30, CWeapon__DoBulletImpact_Hook);

    patch::RedirectCall(0x5FDF7A, CPedIK__PointGunInDirection_Hook);
    patch::RedirectCall(0x61F351, CPedIK__PointGunInDirection_Hook);
    patch::RedirectCall(0x62876D, CPedIK__PointGunInDirection_Hook);

    waypointPlaceEvent += PlaceWaypointHook;
    patch::RedirectCall(0x577582, CRadar__ClearBlip_Hook);

    patch::RedirectCall(0x458475, CExplosion__AddExplosion);
    patch::RedirectCall(0x47752F, CExplosion__AddExplosion);
    patch::RedirectCall(0x47E4E2, CExplosion__AddExplosion);
    patch::RedirectCall(0x5A13B9, CExplosion__AddExplosion);
    patch::RedirectCall(0x6A74EA, CExplosion__AddExplosion);
    patch::RedirectCall(0x6AF0BA, CExplosion__AddExplosion);
    patch::RedirectCall(0x6B3B93, CExplosion__AddExplosion);
    patch::RedirectCall(0x6B3E44, CExplosion__AddExplosion);
    patch::RedirectCall(0x6BEB08, CExplosion__AddExplosion);
    patch::RedirectCall(0x6C703D, CExplosion__AddExplosion);
    patch::RedirectCall(0x6F22BD, CExplosion__AddExplosion);
    patch::RedirectCall(0x737929, CExplosion__AddExplosion);
    patch::RedirectCall(0x737C72, CExplosion__AddExplosion);
    patch::RedirectCall(0x7388B7, CExplosion__AddExplosion);
    patch::RedirectCall(0x738952, CExplosion__AddExplosion);
    patch::RedirectCall(0x738AAE, CExplosion__AddExplosion);

    const int CWorld__Add_Addresses[] = {
    0x404BE4, 0x42BBC3, 0x42C5FC, 0x42CA98, 0x431BE6, 0x4320B1, 0x432239, 0x43A416, 0x43A631, 0x44231F,
    0x444E2C, 0x4456B4, 0x45733A, 0x4575C0, 0x45881B, 0x458E70, 0x45ACBE, 0x45BF05, 0x45CCBB, 0x45ED24,
    0x45ED7B, 0x45F5D5, 0x45F708, 0x45F846, 0x45F979, 0x45FAAC, 0x45FBF3, 0x461820, 0x4621B6, 0x46237F,
    0x462993, 0x467726, 0x469878, 0x469D6D, 0x470E06, 0x47CD5F, 0x47D9B7, 0x47F5EB, 0x4834D6, 0x484FF4,
    0x48A3BB, 0x48DC89, 0x48DD00, 0x48EC41, 0x491DFD, 0x49976D, 0x499A49, 0x544499, 0x546EB8, 0x550138,
    0x556169, 0x592FAE, 0x5936E6, 0x5A1817, 0x5A1833, 0x5A3775, 0x5B16AD, 0x5B5348, 0x5D2D10, 0x5D2E2E,
    0x5D4B1D, 0x5D5125, 0x5E0157, 0x5E41B1, 0x5E4471, 0x60D814, 0x612A10, 0x6136B7, 0x6138DF, 0x6145C7,
    0x61470D, 0x615868, 0x61B059, 0x61B09B, 0x61B117, 0x61B174, 0x61B20E, 0x61B264, 0x66C3CB, 0x6A8BBE,
    0x6A9BE2, 0x6A9D63, 0x6BD080, 0x6C685D, 0x6CD40E, 0x6CD71D, 0x6E44E0, 0x6E4E94, 0x6EADF9, 0x6F2199,
    0x6F2503, 0x6F3C8C, 0x6F7249, 0x6F784B, 0x717E7F, 0x717F3E, 0x738639, 0x15614C6, 0x156D903, 0x156DA51 };
    patch::RedirectJump(0x609554, CWorld__Add_Hook);
    patch::RedirectCall(std::vector<int>(CWorld__Add_Addresses, CWorld__Add_Addresses + sizeof(CWorld__Add_Addresses) / 4), CWorld__Add_Hook);

    const int CWorld__Remove_Addresses[] = {
    0x404B90, 0x404BED, 0x404C3E, 0x409E43, 0x4251E6, 0x425221, 0x42541E, 0x4323F9, 0x4413CA, 0x442319,
    0x449729, 0x4499F3, 0x449B43, 0x449CE0, 0x449E2A, 0x454CFB, 0x455488, 0x4556F1, 0x456C29, 0x456E1E,
    0x456EA0, 0x4571AD, 0x458E8A, 0x45BEFF, 0x45CCB5, 0x45D3C9, 0x45D3FE, 0x45ED11, 0x45ED69, 0x45FF78,
    0x45FFCB, 0x4610CC, 0x4698E4, 0x470DC7, 0x467B3C, 0x47CD0B, 0x486D3E, 0x48A36E, 0x48DC77, 0x48DCEE,
    0x48EC2F, 0x499D90, 0x49A45A, 0x53C98C, 0x54447C, 0x546E2C, 0x5500C5, 0x550127, 0x5567CE, 0x5667B0,
    0x59163E, 0x593794, 0x5A17B4, 0x5A1890, 0x5A18EA, 0x5A194E, 0x5A1A51, 0x5A32A2, 0x5A3FE2, 0x5A82FC,
    0x5AFE6E, 0x5D5112, 0x5E011C, 0x5E03DC, 0x5E4148, 0x5E86AF, 0x6094FC, 0x610F26, 0x612305,
    0x612F1A, 0x6145A4, 0x6146F8, 0x615EA6, 0x615F2D, 0x616404, 0x61AEE3, 0x61AF07, 0x66C3B9, 0x6A9CA4,
    0x6BCFC4, 0x6C7B9A, 0x6CCCB2, 0x6D22D7, 0x6E3E7D, 0x6E3FFD, 0x6E4120, 0x6E4E8B, 0x6E51B4, 0x6F20E4,
    0x6F5DD7, 0x6F6A7B, 0x6F6B31, 0x6F7194, 0x6F7845, 0x6F7ACA, 0x717897, 0x738AFF, 0x73997A, 0x739A17,
    0x739AD0, 0x156DA41 };
    patch::RedirectJump(0x609534, CWorld__Remove_Hook);
    patch::RedirectCall(std::vector<int>(CWorld__Remove_Addresses, CWorld__Remove_Addresses + sizeof(CWorld__Remove_Addresses) / 4), CWorld__Remove_Hook);

    patch::RedirectCall(0x570A1B, CTaskComplexEnterCarAsDriver__Ctor_Hook);
    patch::RedirectCall(0x570A94, CTaskComplexEnterCarAsDriver__Ctor_Hook);

    patch::RedirectCall(0x57049C, CTaskComplexLeaveCar__Ctor_Hook);
    patch::RedirectCall(0x5703F7, CTaskComplexLeaveCar__Ctor_Hook);

    patch::SetPointer(0x871148, CVehicle__ProcessControl_Hook);
    patch::SetPointer(0x8721C8, CVehicle__ProcessControl_Hook);
    patch::SetPointer(0x871388, CVehicle__ProcessControl_Hook);
    patch::SetPointer(0x871970, CVehicle__ProcessControl_Hook);
    patch::SetPointer(0x8716A8, CVehicle__ProcessControl_Hook);
    patch::SetPointer(0x871550, CVehicle__ProcessControl_Hook);
    patch::SetPointer(0x871800, CVehicle__ProcessControl_Hook);
    patch::SetPointer(0x871B10, CVehicle__ProcessControl_Hook);
    patch::SetPointer(0x872398, CVehicle__ProcessControl_Hook);

    // fix CPlayerPed dctor crash
    patch::RedirectCall(0x60A9A3, CPlayerPed__dctor_Hook);

    patch::RedirectCall(0x53C1CB, CCarCtrl__RemoveDistantCars_Hook);


    patch::RedirectCall(0x46DCE4, CDamageManager__ApplyDamage_Hook);
    patch::RedirectCall(0x46DFFD, CDamageManager__ApplyDamage_Hook);
    patch::RedirectCall(0x6A7C2A, CDamageManager__ApplyDamage_Hook);
    patch::RedirectCall(0x6A7DD5, CDamageManager__ApplyDamage_Hook);
    patch::RedirectCall(0x6A7E07, CDamageManager__ApplyDamage_Hook);
    patch::RedirectCall(0x6A7E39, CDamageManager__ApplyDamage_Hook);
    patch::RedirectCall(0x6A7E91, CDamageManager__ApplyDamage_Hook);
    patch::RedirectCall(0x6A7F5B, CDamageManager__ApplyDamage_Hook);
    patch::RedirectCall(0x6A7F8D, CDamageManager__ApplyDamage_Hook);
    patch::RedirectCall(0x6A803F, CDamageManager__ApplyDamage_Hook);

    patch::RedirectCall(0x44828D, CVehicle__SetRemap_Hook);
    patch::RedirectCall(0x44B184, CVehicle__SetRemap_Hook);
    patch::RedirectCall(0x49872F, CVehicle__SetRemap_Hook);

    patch::RedirectJump(0x441390, CGameLogin__IsCoopGameGoingOn_Hook);

    patch::RedirectCall(0x431998, CVehicle__AddVehicleUpgrade_Hook);
    patch::RedirectCall(0x4732DE, CVehicle__AddVehicleUpgrade_Hook);
    patch::RedirectCall(0x4985DA, CVehicle__AddVehicleUpgrade_Hook);
    patch::RedirectCall(0x49B106, CVehicle__AddVehicleUpgrade_Hook);
    patch::RedirectCall(0x6B0BD7, CVehicle__AddVehicleUpgrade_Hook);
    patch::RedirectCall(0x6E3424, CVehicle__AddVehicleUpgrade_Hook);

    patch::RedirectCall(0x4732F2, CVehicle__RemoveVehicleUpgrade_Hook);
    patch::RedirectCall(0x498618, CVehicle__RemoveVehicleUpgrade_Hook);

    patch::RedirectCall(0x6B1F5E, CAutomobile__FireTruckControl_Hook);
    patch::RedirectCall(0x6B2028, CAutomobile__TankControl_Hook);

    patch::RedirectCall(0x62D59B, CTaskSimpleGangDriveBy__SetupStaticAnimForPlayer_Hook);

    patch::RedirectCall(0x62D813, CTaskSimpleGangDriveBy__ProcessAiming_Hook);

    patch::RedirectCall(0x57C2A3, CMenuManager__DrawFrontEnd_FixChat_Hook);
    
    // ped hooks
    patch::RedirectCall(0x53C030, CPopulation__Update_Hook);
    patch::RedirectCall(0x53C054, CPopulation__Update_Hook);

    // ped tasks hooks (help me im going crazy)
    patch::RedirectJump(0x681AF0, CTaskManager__SetTask_Hook);
    patch::RedirectJump(0x681B60, CTaskManager__SetTaskSecondary_Hook);
    patch::RedirectJump(0x61A449, CTaskComplex__SetSubTask_Hook);
    patch::RedirectJump(0x61A660, CTask__dtor_Hook); // todo: not working
    
    // time && weather hooks
    patch::RedirectCall(0x47F1C7, CClock__RestoreClock_Hook);
    patch::RedirectCall(0x441534, CClock__SetGameClock_Hook); 
    patch::RedirectJump(0x43857F, ProcessCheat_Hook);
    patch::RedirectJump(0x47D43E, CWeather__ForceWeather_Hook);
    patch::RedirectJump(0x72A4F0, CWeather__ForceWeatherNow_Hook);
    patch::RedirectCall(0x47679F, CWeather__SetWeatherToAppropriateTypeNow_Hook);
}