#include "stdafx.h"
#include "CRadarSync.h"
#include <CEntryExitManager.h>
#include <CPickups.h>
#include <CObject.h>

static const int UNRESOLVED_ENTITY_HANDLE = -1;

static bool IsEntityBlipType(unsigned int nType)
{ return nType == BLIP_CAR || nType == BLIP_CHAR || nType == BLIP_OBJECT || nType == BLIP_PICKUP; }

// not connected yet, behave like the single player game, the host resyncs everything later
static bool HasAuthority()
{ return CLocalPlayer::m_bIsHost || !CNetwork::m_bAuthenticated; }

static bool CanModifyTrace(int nIndex)
{ return CRadarSync::ms_abLocal[nIndex] || HasAuthority(); }

static bool DoesTraceEntityExist(int nIndex)
{
    tRadarTrace& trace = CRadar::ms_RadarTrace[nIndex];
    int nHandle = trace.m_nEntityHandle;

    if (nHandle == UNRESOLVED_ENTITY_HANDLE)
    {
        return !IsEntityBlipType(trace.m_nBlipType);
    }

    switch (trace.m_nBlipType)
    {
        case BLIP_CAR:
            return CPools::GetVehicle(nHandle) != nullptr;
        case BLIP_CHAR:
            return CPools::GetPed(nHandle) != nullptr;
        case BLIP_OBJECT:
            return CPools::GetObject(nHandle) != nullptr;
        case BLIP_PICKUP:
            return CPickups::GetActualPickupIndex(nHandle) != -1;
    }

    return true;
}

static void ResolveTraceEntity(int nIndex)
{
    tRadarTrace& trace = CRadar::ms_RadarTrace[nIndex];
    trace.m_nEntityHandle = UNRESOLVED_ENTITY_HANDLE;

    switch (trace.m_nBlipType)
    {
        case BLIP_CAR:
        {
            CEntity* pEntity = CRadarSync::ms_aEntity[nIndex].GetEntity();
            if (pEntity != nullptr && pEntity->m_nType == ENTITY_TYPE_VEHICLE)
            {
                trace.m_nEntityHandle = CPools::GetVehicleRef(static_cast<CVehicle*>(pEntity));
            }
            break;
        }
        case BLIP_CHAR:
        {
            CEntity* pEntity = CRadarSync::ms_aEntity[nIndex].GetEntity();
            if (pEntity != nullptr && pEntity->m_nType == ENTITY_TYPE_PED)
            {
                trace.m_nEntityHandle = CPools::GetPedRef(static_cast<CPed*>(pEntity));
            }
            break;
        }
        case BLIP_OBJECT:
        {
            CEntity* pEntity = CRadarSync::ms_aEntity[nIndex].GetEntity();
            if (pEntity != nullptr && pEntity->m_nType == ENTITY_TYPE_OBJECT)
            {
                trace.m_nEntityHandle = CPools::GetObjectRef(static_cast<CObject*>(pEntity));
                break;
            }

            if (CRadarSync::ms_aEntity[nIndex].objectid.valid())
            {
                break;
            }

            for (int i = 0; i < CPools::ms_pObjectPool->m_nSize; i++)
            {
                CObject* pObject = CPools::ms_pObjectPool->GetAt(i);
                if (pObject != nullptr &&
                    DistanceBetweenPoints(pObject->GetPosition(), CRadarSync::ms_avecEntityPosn[nIndex]) < 1.0f)
                {
                    trace.m_nEntityHandle = CPools::GetObjectRef(pObject);
                    break;
                }
            }
            break;
        }
        // TODO: replace with CNetworkEntitySerializer after syncing pickups
        case BLIP_PICKUP:
            for (unsigned int i = 0; i < MAX_NUM_PICKUPS; i++)
            {
                CPickup& pickup = CPickups::aPickUps[i];
                if (pickup.m_nPickupType != PICKUP_NONE &&
                    DistanceBetweenPoints(pickup.GetPosn(), CRadarSync::ms_avecEntityPosn[nIndex]) < 1.0f)
                {
                    trace.m_nEntityHandle = CPickups::GetUniquePickupIndex(i);
                    break;
                }
            }
            break;
    }
}

static CEntryExit* FindEntryExit(int16_t nX, int16_t nY)
{
    for (int i = 0; i < CEntryExitManager::mp_poolEntryExits->m_nSize; i++)
    {
        CEntryExit* pEntryExit = CEntryExitManager::mp_poolEntryExits->GetAt(i);
        if (pEntryExit != nullptr && static_cast<int16_t>(std::floor(pEntryExit->m_recEntrance.left)) == nX &&
            static_cast<int16_t>(std::floor(pEntryExit->m_recEntrance.bottom)) == nY)
        {
            return pEntryExit;
        }
    }

    return nullptr;
}

static void ClearTrace(int nIndex)
{
    bool bLocal = CRadarSync::ms_abLocal[nIndex];

    CRadar::ClearActualBlip(nIndex);
    CRadarSync::ms_abLocal[nIndex] = false;
    CRadarSync::ms_aForWho[nIndex] = playerid_t();

    if (!bLocal)
    {
        CRadarSync::MarkDirty(nIndex);
    }
}

static void OnSharedTraceCreated(int nBlipHandle)
{
    int nIndex = CRadar::GetActualBlipArrayIndex(nBlipHandle);
    if (nIndex == -1)
    {
        return;
    }

    CRadarSync::ms_abLocal[nIndex] = false;
    CRadarSync::ms_aForWho[nIndex] = playerid_t();
    CRadarSync::MarkDirty(nIndex);
}

static void BuildPayload(int nIndex, Packets::Scripts::_RadarTracePayload& payload)
{
    tRadarTrace& trace = CRadar::ms_RadarTrace[nIndex];

    payload = Packets::Scripts::_RadarTracePayload();
    payload.counter = trace.m_nCounter;
    payload.bInUse = trace.m_bInUse && !CRadarSync::ms_abLocal[nIndex];

    // if the blip is disabled, dont build the rest
    if (!payload.bInUse)
    {
        return;
    }

    payload.colour = trace.m_nColour;
    payload.type = trace.m_nBlipType;
    payload.display = trace.m_nBlipDisplay;
    payload.sprite = trace.m_nRadarSprite;
    payload.coordBlipAppearance = trace.m_nCoordBlipAppearance;
    payload.bBright = trace.m_bBright;
    payload.bShortRange = trace.m_bShortRange;
    payload.bFriendly = trace.m_bFriendly;
    payload.bBlipRemain = trace.m_bBlipRemain;
    payload.bBlipFade = trace.m_bBlipFade;
    payload.size = trace.m_nBlipSize;
    payload.sphereRadius = trace.m_fSphereRadius;

    int nHandle = trace.m_nEntityHandle;
    bool bResolved = nHandle != UNRESOLVED_ENTITY_HANDLE;

    switch (trace.m_nBlipType)
    {
        case BLIP_CAR:
        {
            CVehicle* pVehicle = bResolved ? CPools::GetVehicle(nHandle) : nullptr;
            if (pVehicle != nullptr)
            {
                CRadarSync::ms_aEntity[nIndex].SetEntity(pVehicle);
            }
            payload.entity = CRadarSync::ms_aEntity[nIndex];
            break;
        }
        case BLIP_CHAR:
        {
            CPed* pPed = bResolved ? CPools::GetPed(nHandle) : nullptr;
            if (pPed != nullptr)
            {
                CRadarSync::ms_aEntity[nIndex].SetEntity(pPed);
            }
            payload.entity = CRadarSync::ms_aEntity[nIndex];
            break;
        }
        case BLIP_OBJECT:
        {
            CObject* pObject = bResolved ? CPools::GetObject(nHandle) : nullptr;
            if (pObject != nullptr)
            {
                CRadarSync::ms_aEntity[nIndex].SetEntity(pObject);
                CRadarSync::ms_avecEntityPosn[nIndex] = pObject->GetPosition();
            }
            payload.entity = CRadarSync::ms_aEntity[nIndex];
            payload.position = CRadarSync::ms_avecEntityPosn[nIndex];
            break;
        }
        case BLIP_PICKUP:
        {
            int nPickupIndex = bResolved ? CPickups::GetActualPickupIndex(nHandle) : -1;
            if (nPickupIndex != -1)
            {
                CRadarSync::ms_avecEntityPosn[nIndex] = CPickups::aPickUps[nPickupIndex].GetPosn();
            }
            payload.position = CRadarSync::ms_avecEntityPosn[nIndex];
            break;
        }
        default:
            payload.position = trace.m_vecPos;
            break;
    }

    if (trace.m_pEntryExit != nullptr)
    {
        payload.bHasEntryExit = true;
        payload.entryExitX = static_cast<int16_t>(std::floor(trace.m_pEntryExit->m_recEntrance.left));
        payload.entryExitY = static_cast<int16_t>(std::floor(trace.m_pEntryExit->m_recEntrance.bottom));
    }

    payload.forWho = CRadarSync::ms_aForWho[nIndex];
}

static void ApplyPayload(int nIndex, const Packets::Scripts::_RadarTracePayload& payload)
{
    tRadarTrace& trace = CRadar::ms_RadarTrace[nIndex];

    // the blip was removed
    if (!payload.bInUse)
    {
        if (CRadarSync::ms_abLocal[nIndex])
        {
            return;
        }

        if (trace.m_bInUse)
        {
            CRadar::ClearActualBlip(nIndex);
        }
        trace.m_nCounter = payload.counter;
        CRadarSync::ms_aForWho[nIndex] = playerid_t();
        return;
    }

    CRadarSync::ms_abLocal[nIndex] = false;

    trace.m_nCounter = payload.counter;
    trace.m_nColour = payload.colour;
    trace.m_nBlipType = payload.type;
    trace.m_nBlipDisplay = payload.display;
    trace.m_nRadarSprite = payload.sprite;
    trace.m_nCoordBlipAppearance = payload.coordBlipAppearance;
    trace.m_bBright = payload.bBright;
    trace.m_bInUse = true;
    trace.m_bShortRange = payload.bShortRange;
    trace.m_bFriendly = payload.bFriendly;
    trace.m_bBlipRemain = payload.bBlipRemain;
    trace.m_bBlipFade = payload.bBlipFade;
    trace.m_nBlipSize = payload.size;
    trace.m_fSphereRadius = payload.sphereRadius;
    trace.m_pEntryExit = payload.bHasEntryExit ? FindEntryExit(payload.entryExitX, payload.entryExitY) : nullptr;
    trace.m_nEntityHandle = 0;

    switch (payload.type)
    {
        case BLIP_CAR:
        case BLIP_CHAR:
            CRadarSync::ms_aEntity[nIndex] = payload.entity;
            ResolveTraceEntity(nIndex);
            break;
            
        case BLIP_OBJECT:
            CRadarSync::ms_aEntity[nIndex] = payload.entity;
            CRadarSync::ms_avecEntityPosn[nIndex] = payload.position;
            ResolveTraceEntity(nIndex);
            break;

        // TODO: replace with CNetworkEntitySerializer after syncing pickups
        case BLIP_PICKUP:
            CRadarSync::ms_avecEntityPosn[nIndex] = payload.position;
            ResolveTraceEntity(nIndex);
            break;
        default:
            trace.m_vecPos = payload.position;
    }

    CRadarSync::ms_aForWho[nIndex] = payload.forWho;
}

static int FindFreeLocalTraceIndex()
{
    for (int i = MAX_RADAR_TRACES - 1; i >= 0; i--)
    {
        if (!CRadar::ms_RadarTrace[i].m_bInUse)
        {
            return i;
        }
    }

    return -1;
}

static void InitLocalTrace(int nIndex, eBlipType type, eBlipDisplay display)
{
    tRadarTrace& trace = CRadar::ms_RadarTrace[nIndex];
    trace.m_nBlipType = type;
    trace.m_nBlipDisplay = display;
    trace.m_fSphereRadius = 1.0f;
    trace.m_nBlipSize = 1;
    trace.m_nRadarSprite = RADAR_SPRITE_NONE;
    trace.m_bBright = true;
    trace.m_bInUse = true;
    trace.m_bShortRange = false;
    trace.m_bFriendly = false;
    trace.m_bBlipRemain = false;
    trace.m_bBlipFade = false;
    trace.m_nCoordBlipAppearance = 0;
    trace.m_pEntryExit = nullptr;

    CRadarSync::ms_abLocal[nIndex] = true;
    CRadarSync::ms_aForWho[nIndex] = playerid_t();
}

int CRadarSync::SetLocalCoordBlip(eBlipType type, CVector vecPosn, unsigned int nColour, eBlipDisplay display)
{
    int nIndex = FindFreeLocalTraceIndex();
    if (nIndex == -1)
    {
        return -1;
    }

    InitLocalTrace(nIndex, type, display);
    CRadar::ms_RadarTrace[nIndex].m_vecPos = vecPosn;
    CRadar::ms_RadarTrace[nIndex].m_nColour = BLIP_COLOUR_DESTINATION;
    CRadar::ms_RadarTrace[nIndex].m_nEntityHandle = 0;

    return CRadar::GetNewUniqueBlipIndex(nIndex);
}

int CRadarSync::SetLocalEntityBlip(eBlipType type, int nEntityHandle, unsigned int nColour, eBlipDisplay display)
{
    int nIndex = FindFreeLocalTraceIndex();
    if (nIndex == -1)
    {
        return -1;
    }

    InitLocalTrace(nIndex, type, display);
    CRadar::ms_RadarTrace[nIndex].m_nColour =
        (type == BLIP_CHAR || type == BLIP_CAR) ? BLIP_COLOUR_THREAT : BLIP_COLOUR_GREEN;
    CRadar::ms_RadarTrace[nIndex].m_nEntityHandle = nEntityHandle;

    return CRadar::GetNewUniqueBlipIndex(nIndex);
}

void CRadarSync::ClearLocalBlip(int nBlipHandle)
{
    int nIndex = CRadar::GetActualBlipArrayIndex(nBlipHandle);
    if (nIndex != -1 && ms_abLocal[nIndex])
    {
        ClearTrace(nIndex);
    }
}

static void __cdecl CRadar__ClearActualBlip_Hook_Local(int nIndex)
{
    if (nIndex >= 0 && nIndex < MAX_RADAR_TRACES && CRadarSync::ms_abLocal[nIndex])
    {
        ClearTrace(nIndex);
    }
}

static int __cdecl CRadar__SetCoordBlip_Hook_Local(
    eBlipType type, CVector vecPosn, unsigned int nColour, eBlipDisplay display)
{ return CRadarSync::SetLocalCoordBlip(type, vecPosn, nColour, display); }

static int __cdecl CRadar__SetEntityBlip_Hook_Local(
    eBlipType type, int nEntityHandle, unsigned int nColour, eBlipDisplay display)
{ return CRadarSync::SetLocalEntityBlip(type, nEntityHandle, nColour, display); }

static int __cdecl CRadar__SetCoordBlip_Hook(
    eBlipType type, CVector vecPosn, unsigned int nColour, eBlipDisplay display)
{
    if (!HasAuthority())
    {
        return -1;
    }

    int nBlipHandle = CRadar::SetCoordBlip(type, vecPosn, nColour, display, nullptr);
    OnSharedTraceCreated(nBlipHandle);
    return nBlipHandle;
}

static int __cdecl CRadar__SetShortRangeCoordBlip_Hook(
    eBlipType type, CVector vecPosn, unsigned int nColour, eBlipDisplay display)
{
    if (!HasAuthority())
    {
        return -1;
    }

    int nBlipHandle = CRadar::SetShortRangeCoordBlip(type, vecPosn, nColour, display, nullptr);
    OnSharedTraceCreated(nBlipHandle);
    return nBlipHandle;
}

static int __cdecl CRadar__SetEntityBlip_Hook(
    eBlipType type, int nEntityHandle, unsigned int nColour, eBlipDisplay display)
{
    if (!HasAuthority())
    {
        return -1;
    }

    int nBlipHandle = CRadar::SetEntityBlip(type, nEntityHandle, nColour, display);
    OnSharedTraceCreated(nBlipHandle);
    return nBlipHandle;
}

template <typename T, void (*pfnOriginal)(int, T)>
static void __cdecl CRadar__ModifyBlip_Hook(int nBlipHandle, T value)
{
    int nIndex = CRadar::GetActualBlipArrayIndex(nBlipHandle);
    if (nIndex == -1 || !CanModifyTrace(nIndex))
    {
        return;
    }

    pfnOriginal(nBlipHandle, value);
    CRadarSync::MarkDirty(nIndex);
}

static void __cdecl CRadar__ClearBlip_Hook(int nBlipHandle)
{
    int nIndex = CRadar::GetActualBlipArrayIndex(nBlipHandle);
    if (nIndex != -1 && CanModifyTrace(nIndex))
    {
        ClearTrace(nIndex);
    }
}

static void __cdecl CRadar__ClearBlipForEntity_Hook(eBlipType type, int nEntityHandle)
{
    for (int i = 0; i < MAX_RADAR_TRACES; i++)
    {
        tRadarTrace& trace = CRadar::ms_RadarTrace[i];
        if (trace.m_nBlipType == type && trace.m_nEntityHandle == nEntityHandle && CanModifyTrace(i))
        {
            ClearTrace(i);
        }
    }
}

static void __cdecl CRadar__DrawEntityBlip_Hook(int nIndex, unsigned char bDrawSprite)
{
    if (CRadarSync::IsTraceVisible(nIndex))
    {
        CRadar::DrawEntityBlip(nIndex, bDrawSprite);
    }
}

static void __cdecl CRadar__DrawCoordBlip_Hook(int nIndex, unsigned char bDrawSprite)
{
    if (CRadarSync::IsTraceVisible(nIndex))
    {
        CRadar::DrawCoordBlip(nIndex, bDrawSprite);
    }
}

// there is no per-blip function in Draw3dMarkers, hide the invisible traces while it is running
static void __cdecl CRadar__Draw3dMarkers_Hook()
{
    bool abHidden[MAX_RADAR_TRACES];

    for (int i = 0; i < MAX_RADAR_TRACES; i++)
    {
        abHidden[i] = CRadar::ms_RadarTrace[i].m_bInUse && !CRadarSync::IsTraceVisible(i);
        if (abHidden[i])
        {
            CRadar::ms_RadarTrace[i].m_bInUse = false;
        }
    }

    CRadar::Draw3dMarkers();

    for (int i = 0; i < MAX_RADAR_TRACES; i++)
    {
        if (abHidden[i])
        {
            CRadar::ms_RadarTrace[i].m_bInUse = true;
        }
    }
}

bool CRadarSync::IsTraceVisible(int nIndex)
{
    if (ms_aForWho[nIndex].valid() && ms_aForWho[nIndex] != CNetworkPlayerManager::m_localPlayerId)
    {
        return false;
    }

    if (!IsEntityBlipType(CRadar::ms_RadarTrace[nIndex].m_nBlipType))
    {
        return true;
    }

    if (!ms_abLocal[nIndex] && !HasAuthority() && !DoesTraceEntityExist(nIndex))
    {
        ResolveTraceEntity(nIndex);
    }

    return DoesTraceEntityExist(nIndex);
}

void CRadarSync::SetBlipVisibleTo(int nBlipHandle, CPed* pPed)
{
    if (!HasAuthority())
    {
        return;
    }

    int nIndex = CRadar::GetActualBlipArrayIndex(nBlipHandle);
    if (nIndex == -1 || ms_abLocal[nIndex])
    {
        return;
    }

    playerid_t playerid;
    if (pPed == FindPlayerPed(0))
    {
        playerid = CNetworkPlayerManager::m_localPlayerId;
    }
    else if (pPed != nullptr)
    {
        if (CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(pPed))
        {
            playerid = pNetworkPlayer->m_playerId;
        }
    }

    ms_aForWho[nIndex] = playerid;
    MarkDirty(nIndex);
}

void CRadarSync::MarkDirty(int nIndex)
{
    if (CLocalPlayer::m_bIsHost && !ms_abLocal[nIndex])
    {
        ms_abDirty[nIndex] = true;
    }
}

void CRadarSync::SendChangedTraces()
{
    for (int i = 0; i < MAX_RADAR_TRACES; i++)
    {
        if (!ms_abDirty[i])
        {
            continue;
        }

        ms_abDirty[i] = false;

        if (!CLocalPlayer::m_bIsHost)
        {
            continue;
        }

        Packets::Scripts::RadarTraceUpdate packet;
        packet.index = static_cast<uint8_t>(i);
        BuildPayload(i, packet.trace);
        GetPacketFactory().Send(packet);
    }
}

void CRadarSync::SendAllTraces()
{
    static Packets::Scripts::UpdateAllRadarTraces packet;

    for (int i = 0; i < MAX_RADAR_TRACES; i++)
    {
        ms_abDirty[i] = false;
        BuildPayload(i, packet.traces[i]);
    }

    GetPacketFactory().Send(packet);
}

void CRadarSync::HandlePacket(const Packets::Scripts::RadarTraceUpdate& packet)
{
    if (CLocalPlayer::m_bIsHost)
    {
        return;
    }

    ApplyPayload(packet.index, packet.trace);
}

void CRadarSync::HandlePacket(const Packets::Scripts::UpdateAllRadarTraces& packet)
{
    if (CLocalPlayer::m_bIsHost)
    {
        return;
    }

    for (int i = 0; i < MAX_RADAR_TRACES; i++)
    {
        ApplyPayload(i, packet.traces[i]);
    }
}

void CRadarSync::InjectHooks()
{
    patch::RedirectCall(0x587F45, CRadar__SetCoordBlip_Hook_Local);     // CRadar::SetupAirstripBlips
    patch::RedirectCall(0x60CB61, CRadar__SetEntityBlip_Hook_Local);    // CPlayerPed::MakeThisPedJoinOurGroup
    patch::RedirectCall(0x738753, CRadar__SetEntityBlip_Hook_Local);    // CProjectileInfo::AddProjectile
    patch::RedirectCall(0x573372, CRadarSync::ClearLocalBlip);          // CMenuManager::DoSettingsBeforeStartingAGame
    patch::RedirectCall(0x587EE0, CRadarSync::ClearLocalBlip);          // CRadar::SetupAirstripBlips
    patch::RedirectCall(0x587F96, CRadar__ClearActualBlip_Hook_Local);  // CRadar::SetupAirstripBlips

    patch::RedirectCall({0x4443D5, 0x47C744, 0x47CFE0, 0x47F74C, 0x47F7EF}, CRadar__SetCoordBlip_Hook);
    patch::RedirectCall({0x48BC9A, 0x48DBD2}, CRadar__SetShortRangeCoordBlip_Hook);
    patch::RedirectCall(
        {0x444F4E, 0x445349, 0x44582F, 0x471D52, 0x47C640, 0x47CEE6, 0x4854EE, 0x497BFE}, CRadar__SetEntityBlip_Hook);

    patch::RedirectCall(
        {0x444F5F, 0x44535A, 0x445840, 0x47C698}, &CRadar__ModifyBlip_Hook<unsigned int, &CRadar::ChangeBlipColour>);
    patch::RedirectCall({0x444F58, 0x445353, 0x445839, 0x471D5C, 0x47C77A, 0x47CEF0, 0x47CFEA, 0x4854F8, 0x497C08},
        &CRadar__ModifyBlip_Hook<int, &CRadar::ChangeBlipScale>);
    patch::RedirectCall(
        {0x446EA8, 0x446EB5, 0x47D022}, &CRadar__ModifyBlip_Hook<eBlipDisplay, &CRadar::ChangeBlipDisplay>);
    patch::RedirectCall(
        {0x444403, 0x47F7FE, 0x48BCA8, 0x48DBE1}, &CRadar__ModifyBlip_Hook<int, &CRadar::SetBlipSprite>);
    patch::RedirectCall(0x46C692, &CRadar__ModifyBlip_Hook<unsigned char, &CRadar::SetBlipAlwaysDisplayInZoom>);
    patch::RedirectCall(0x476114, &CRadar__ModifyBlip_Hook<unsigned char, &CRadar::SetCoordBlipAppearance>);
    patch::RedirectCall(0x47285D, &CRadar__ModifyBlip_Hook<unsigned char, &CRadar::SetBlipFriendly>);
    patch::RedirectCall(0x475882, &CRadar__ModifyBlip_Hook<CEntryExit*, &CRadar::SetBlipEntryExit>);

    patch::RedirectCall({0x446EC0, 0x47C670}, CRadar__ClearBlip_Hook);
    patch::RedirectCall({0x445CFD, 0x445DE6, 0x4556E1, 0x456C19, 0x456E0F, 0x456E91, 0x59F735, 0x5E4564, 0x5E7C88,
                            0x5E86E0, 0x5F812E, 0x6E2BB2, 0x738AF9, 0x739974, 0x739A11, 0x739AC3},
        CRadar__ClearBlipForEntity_Hook);

    patch::RedirectCall(0x58837C, CRadar__DrawEntityBlip_Hook);
    patch::RedirectCall({0x588347, 0x588411}, CRadar__DrawCoordBlip_Hook);
    patch::RedirectCall(0x58FCB8, CRadar__Draw3dMarkers_Hook);
}
