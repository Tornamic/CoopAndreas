#include "stdafx.h"

CPad* __cdecl CPad__GetPad_Hook(int number)
{
	return &CNetworkPlayerManager::m_pPads[0];
}

bool pPressingDuck[MAX_SERVER_PLAYERS + 2] = {false};

void __fastcall CPlayerPed__ProcessControl_Hook(CPlayerPed* This)
{
    CPlayerPed* localPlayer = FindPlayerPed(0);
     
    if (This == localPlayer)
    {
        plugin::CallMethod<0x60EA90, CPlayerPed*>(This);
        return;
    }

    CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(This);

    if (player == nullptr)
        return;

    CWorld::PlayerInFocus = player->GetInternalId();

    CPad* pad = This->GetPadFromPlayer();

    CControllerState newOldState = pad->NewState;
    CControllerState oldOldState = pad->OldState;

    CAMERA_AIM oldCameraState = *(CAMERA_AIM*)&TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecFront;
    eCamMode oldCameraMode = TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode;

    if (player->m_lOnFoot != nullptr && player->m_oOnFoot != nullptr)
    {
        *(CAMERA_AIM*)&TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecFront = player->m_lOnFoot->aim;

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

        

        TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode = (eCamMode)player->m_lOnFoot->cameraMode;

        player->m_pPed->m_vecMoveSpeed = player->m_lOnFoot->velocity;
    }
    
    plugin::CallMethod<0x60EA90, CPlayerPed*>(This);

    CWorld::PlayerInFocus = 0;

    pad->NewState = newOldState;
    pad->OldState = oldOldState;

    *(CAMERA_AIM*)&TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecFront = oldCameraState;
    TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode = oldCameraMode;
}

static void __fastcall CWeapon__DoBulletImpact_Hook(CWeapon* weapon, int padding, CEntity* owner, CEntity* victim, CVector* startPoint, CVector* endPoint, CColPoint* colPoint, int incrementalHit)
{
    if (owner == FindPlayerPed(0))
    {
        CPackets::PlayerBulletShot* packet = new CPackets::PlayerBulletShot;

        CNetworkPlayer* target = CNetworkPlayerManager::GetPlayer(victim);

        if (target == nullptr)
            packet->targetid = -1;
        else
            packet->targetid = target->m_iPlayerId;

        packet->startPos = *startPoint;
        packet->endPos = *endPoint;
        packet->colPoint = *colPoint;
        packet->incrementalHit = incrementalHit;
        CNetwork::SendPacket(CPacketsID::PLAYER_BULLET_SHOT, packet, sizeof * packet, ENET_PACKET_FLAG_UNSEQUENCED);

        weapon->DoBulletImpact(owner, victim, startPoint, endPoint, colPoint, incrementalHit);

        return;
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
}
