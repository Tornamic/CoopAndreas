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

    if (player->m_lOnFoot != nullptr && player->m_oOnFoot != nullptr)
    {
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
    }
    
    plugin::CallMethod<0x60EA90, CPlayerPed*>(This);

    CWorld::PlayerInFocus = 0;

    pad->NewState = newOldState;
    pad->OldState = oldOldState;
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
    CPackets::PlayerPlaceWaypoint packet = { 0, false, CVector(0, 0, 0)};
    CNetwork::SendPacket(CPacketsID::PLAYER_PLACE_WAYPOINT, &packet, sizeof packet, ENET_PACKET_FLAG_RELIABLE);
    CRadar::ClearBlip(blipIndex);
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
}
