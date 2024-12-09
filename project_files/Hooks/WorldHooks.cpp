#include "stdafx.h"
#include "WorldHooks.h"
#include "CNetworkVehicle.h"
#include "CNetworkPed.h"

#ifdef DEBUG_NOT_SYNCED_VEHICLES
std::unordered_map<CVehicle*, std::vector<void*>> WorldHooks::m_aVehicleStackMap;
#endif

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
            CNetworkVehicle* networkVehicle = CNetworkVehicle::CreateHosted(vehicle);
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

    //if (dontCreateEntity)
    //{
    //    //delete entity;
    //    return;
    //}

#ifdef DEBUG_NOT_SYNCED_VEHICLES
    if (entity->m_nType == ENTITY_TYPE_VEHICLE)
    {
        void* stack[10];
        unsigned short frames = CaptureStackBackTrace(0, 10, stack, nullptr);

        WorldHooks::m_aVehicleStackMap[(CVehicle*)entity] = std::vector<void*>(stack, stack + frames);
    }
#endif


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

    if (entity && (unsigned int)*(void***)entity != 0x863C40)
    {
        CWorld::Remove(entity);
    }
}

void WorldHooks::InjectHooks()
{
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
    0x404B90, 0x404BED, 0x404C3E, 0x409E43, 0x4251E6, 0x425221, 0x42541E, 0x4413CA, 0x442319,
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

    patch::RedirectJump(0x47D43E, CWeather__ForceWeather_Hook);
    patch::RedirectJump(0x72A4F0, CWeather__ForceWeatherNow_Hook);
    patch::RedirectCall(0x47679F, CWeather__SetWeatherToAppropriateTypeNow_Hook);
}
