#include "stdafx.h"
#include "CAimSync.h"
#include <CPacketBuffer.h>
#include <CServerTime.h>

Packets::Players::PlayerCameraSync storedCameraState{};
eCamMode savedPlayerWeaponCamMode;

bool Is1stPersonMode(uint16_t camMode)
{
    return camMode == MODE_SNIPER || camMode == MODE_M16_1STPERSON || camMode == MODE_ROCKETLAUNCHER ||
           camMode == MODE_ROCKETLAUNCHER_HS || camMode == MODE_HELICANNON_1STPERSON || camMode == MODE_CAMERA;
}

bool IsWeaponRelatedCamMode(uint16_t camMode)
{
    return camMode == MODE_AIMWEAPON || camMode == MODE_AIMWEAPON_ATTACHED || camMode == MODE_AIMWEAPON_FROMCAR ||
           Is1stPersonMode(camMode);
}

void CollectState(Packets::Players::PlayerCameraSync* pOut)
{
    const CCam& cam = TheCamera.m_aCams[TheCamera.m_nActiveCam];

    pOut->cameraMode = cam.m_nMode;
    savedPlayerWeaponCamMode = (eCamMode)TheCamera.m_PlayerWeaponMode.m_nMode;
    pOut->cameraFov = cam.m_fFOV;
    pOut->front = cam.m_vecFront;
    pOut->source = cam.m_vecSource;
    pOut->up = cam.m_vecUp;

    if (Is1stPersonMode(pOut->cameraMode))
    {
        pOut->lookPitch = TheCamera.Find3rdPersonQuickAimPitch();
    }
    else
    {
        pOut->lookPitch = FindPlayerPed(0)->m_pPlayerData->m_fLookPitch;
    }

    pOut->orientation = TheCamera.m_fOrientation;
}

void ApplyPacketToGame(const Packets::Players::PlayerCameraSync& packet)
{
    CCam& cam = TheCamera.m_aCams[TheCamera.m_nActiveCam];

    cam.m_fFOV = packet.cameraFov;
    cam.m_nMode = (eCamMode)packet.cameraMode;

    if (CWorld::PlayerInFocus == 0)
    {
        TheCamera.m_PlayerWeaponMode.m_nMode = savedPlayerWeaponCamMode;
    }
    else
    {
        switch (cam.m_nMode)
        {
            case MODE_CAMERA:
            case MODE_AIMWEAPON_ATTACHED:
            case MODE_HELICANNON_1STPERSON:
            case MODE_ROCKETLAUNCHER:
            case MODE_ROCKETLAUNCHER_HS:
            case MODE_SNIPER:
            case MODE_M16_1STPERSON:
            case MODE_AIMWEAPON_FROMCAR:
            case MODE_AIMWEAPON:
                TheCamera.m_PlayerWeaponMode.m_nMode = cam.m_nMode;
                break;
            default:
                TheCamera.m_PlayerWeaponMode.m_nMode = MODE_NONE;
                break;
        }
    }

    cam.m_vecFront = packet.front;
    cam.m_vecSource = packet.source;
    cam.m_vecUp = packet.up;
    FindPlayerPed()->m_pPlayerData->m_fLookPitch = packet.lookPitch;
    TheCamera.m_fOrientation = packet.orientation.m_angle;
}

void ApplyPacketInterpolated(CNetworkPlayer* pNetworkPlayer, const Packets::Players::PlayerCameraSync& packetA,
    const Packets::Players::PlayerCameraSync& packetB)
{
    Packets::Players::PlayerCameraSync interp{};

    if (packetB.serverTime == packetA.serverTime)
    {
        interp = packetB;
    }
    else
    {
        float t = (float)(GetTickCount() - pNetworkPlayer->m_startedInterpolatingCameraAt) /
                  (float)(packetB.serverTime - packetA.serverTime);
        t = std::clamp(t, 0.0f, 1.0f);
        interp.cameraFov = Lerp(t, packetA.cameraFov, packetB.cameraFov);
        interp.cameraMode = packetA.cameraMode;
        interp.front = LerpVector(t, packetA.front, packetB.front);
        interp.source = LerpVector(t, packetA.source, packetB.source);
        interp.up = LerpVector(t, packetA.up, packetB.up);
        interp.lookPitch = Lerp(t, packetA.lookPitch, packetB.lookPitch);
        interp.orientation.m_angle = -LerpAngle(t, -packetA.orientation.m_angle, -packetB.orientation.m_angle);
    }

    CPed* pPed = pNetworkPlayer->m_pPed;
    // TODO find a better way to smooth currentRotation, also it is not so accurate
    // if aiming, calculate the player's heading depending on the camera vectors
    if (Is1stPersonMode(interp.cameraMode))
    {
        // 0x511B1B
        float CamDirection = atan2f(-interp.front.x, interp.front.y);
        pNetworkPlayer->m_pPed->m_fAimingRotation = pNetworkPlayer->m_pPed->m_fCurrentRotation = CamDirection;
    }
    else if (interp.cameraMode == MODE_AIMWEAPON)
    {
        // 0x522C37
        eWeaponType weaponType = WEAPON_UNARMED;
        CWeaponInfo* pWeaponInfo = nullptr;
        if (pPed->m_pIntelligence->GetTaskUseGun())
        {
            pWeaponInfo = pPed->m_pIntelligence->GetTaskUseGun()->m_pWeaponInfo;
        }
        else
        {
            weaponType = pPed->m_aWeapons[pPed->m_nActiveWeaponSlot].m_eWeaponType;
            pWeaponInfo = CWeaponInfo::GetWeaponInfo(weaponType, 1);
        }

        float fAimAngleBeta;
        if (pWeaponInfo && pWeaponInfo->m_nWeaponFire == WEAPON_FIRE_MELEE)
        {
            fAimAngleBeta = DEGTORAD(2.0f);
        }
        else
        {
            const float fScreenAngle = DEGTORAD(0.5f * interp.cameraFov);
            const float fScreenPosMult = 2.0f * (CCamera::m_f3rdPersonCHairMultX - 0.5f);
            fAimAngleBeta = atanf(fScreenPosMult * tanf(fScreenAngle));
        }

        if (pWeaponInfo && (!pWeaponInfo->m_nFlags.bAimWithArm || pPed->m_nPedFlags.bIsDucking) &&
            pWeaponInfo->m_nWeaponFire != WEAPON_FIRE_MELEE && !pPed->m_nPedFlags.bInVehicle)
        {
            float CamDirection = atan2f(-interp.front.x, interp.front.y) - fAimAngleBeta;
            if (CamDirection > -100.0f)
            {
                pNetworkPlayer->m_pPed->m_fAimingRotation = pNetworkPlayer->m_pPed->m_fCurrentRotation =
                    CamDirection - 0.05f;
            }
        }
    }

    ApplyPacketToGame(interp);
}

void CAimSync::ApplyNetworkPlayerContext(CNetworkPlayer* player)
{
    CollectState(&storedCameraState);
    //ApplyPacketToGame(player->m_cameraSnapshot);
    ApplyPacketInterpolated(player, player->m_cameraSnapshotOld, player->m_cameraSnapshot);
    if (TheCamera.m_PlayerWeaponMode.m_nMode == MODE_FOLLOWPED/* || TheCamera.m_PlayerWeaponMode.m_nMode ==
     MODE_SNIPER*/)
    {
        TheCamera.m_PlayerWeaponMode.m_nMode = MODE_NONE;
    }

    // camera context switching patches, fix right click freeze

    // disable CCamera::ClearPlayerWeaponMode
    // ret
    // patch::SetUChar(0x50AB10, 0xC3, false);

    //// disable CCamera::SetNewPlayerWeaponMode
    //// ret    0xC
    // patch::SetRaw(0x50BFB0, (void*)"\xC2\x0C\x00", 3, false);

    //// disable CCamera::Using1stPersonWeaponMode
    //// mov    al,0x1
    //// ret
    // patch::SetRaw(0x50BFF0, (void*)"\xB0\x01\xC3", 3, false);

    //// disable CPlayerPed::ClearWeaponTarget
    //// ret
    // patch::SetUChar(0x609C80, 0xC3, false);
}

void CAimSync::ApplyLocalContext()
{
    // return original bytes
    // patch::SetUChar(0x50AB10, 0x33, false);                    // CCamera::ClearPlayerWeaponMode
    // patch::SetRaw(0x50BFB0, (void*)"\x66\x8B\x44", 3, false);  // CCamera::SetNewPlayerWeaponMode
    // patch::SetRaw(0x50BFF0, (void*)"\x66\x8B\x81", 3, false);  // CCamera::Using1stPersonWeaponMode
    // patch::SetUChar(0x609C80, 0x57, false);                    // CPlayerPed::ClearWeaponTarget

    ApplyPacketToGame(storedCameraState);
}

void GetCameraResyncRate(uint32_t& nResyncRate)
{
    assert(CWorld::PlayerInFocus == 0);

    CPad* pPad = CPad::GetPad(0);

    if ((pPad->NewMouseControllerState.x == 0.0f || pPad->NewMouseControllerState.y == 0.0f) &&
        (pPad->OldMouseControllerState.x != 0.0f || pPad->NewMouseControllerState.y != 0.0f))
    {
        // just stopped moving the mouse
        // send faster if aiming, send immediately if not
        if (IsWeaponRelatedCamMode(TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode))
        {
            nResyncRate = 50;
        }
        else
        {
            nResyncRate = 0;
        }
        return;
    }

    // actively moving the mouse, update frequently
    if (pPad->NewMouseControllerState.x != 0.0f || pPad->NewMouseControllerState.y != 0.0f)
    {
        // aim is interpolated, regular camera update is not, so do it a little faster

        if (IsWeaponRelatedCamMode(TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode))
        {
            nResyncRate = 100;
        }
        else
        {
            nResyncRate = 66;
        }
        return;
    }

    // not moving the mouse, update passively
    nResyncRate = 333;
}

void CAimSync::ProcessSyncing()
{
    static uint32_t lastPlayerCameraSyncTick = 0;
    static Packets::Players::PlayerCameraSync lastSentState{};

    uint32_t tickCount = CTimer::m_snTimeInMilliseconds;
    CPlayerPed* pPlayerPed = FindPlayerPed(0);

    uint32_t nResyncRate;
    GetCameraResyncRate(nResyncRate);

    if (tickCount - lastPlayerCameraSyncTick > nResyncRate)
    {
        bool requireFullUpdate = false;
        if (IsWeaponRelatedCamMode(TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode))
        {
            requireFullUpdate = true;
        }
        else if (pPlayerPed->m_pVehicle && pPlayerPed->m_nPedFlags.bInVehicle &&
                 pPlayerPed->m_pVehicle->m_pDriver == pPlayerPed && pPlayerPed->m_pVehicle->HasTurret())
        {
            requireFullUpdate = true;
        }

        Packets::Players::PlayerCameraSync cameraState;
        CollectState(&cameraState);
        cameraState.bFullUpdate = requireFullUpdate;

        if (cameraState != lastSentState)
        {
            GetPacketFactory().Send(cameraState);
            lastSentState = cameraState;
            lastPlayerCameraSyncTick = tickCount;
        }
    }
}
