#include "stdafx.h"
#include "CEntryExitTransitionSync.h"
#include "CEntryExitManager.h"
#include <CBuilding.h>
#include <game_sa/CTaskComplexGotoDoorAndOpen.h>
#include <game_sa/CTaskSimpleUninterruptable.h>

namespace
{
constexpr DWORD DOOR_CLOSE_TIMEOUT_MS = 10000;
constexpr float CLOSED_MATRIX_DIFFERENCE_SQ = 0.0004f;

struct DoorGuard
{
    CEntity* m_pDoor = nullptr;
    CBuilding* m_pBlocker = nullptr;
    CMatrix m_matrixClosed{};
    int m_nActiveTransitions = 0;
    DWORD m_nNoLeaseSince = 0;
};

struct CollisionParticipant
{
    CEntity* m_pPed = nullptr;
    CEntity* m_pPreviousIgnoredCollision = nullptr;
    DoorGuard* m_pGuard = nullptr;
};

struct RemoteTransition
{
    CollisionParticipant m_participant{};
    CPlayerPed* m_pPed = nullptr;
    CTask* m_pTask = nullptr;
};

using DoorGuardMap = std::unordered_map<CEntity*, DoorGuard>;
using RemoteTransitionMap = std::unordered_map<int, RemoteTransition>;

DoorGuardMap g_doorGuards;
RemoteTransitionMap g_remoteTransitions;
CollisionParticipant g_localParticipant;

float VectorDifferenceSq(const CVector& vecLeft, const CVector& vecRight)
{
    const float fX = vecLeft.x - vecRight.x;
    const float fY = vecLeft.y - vecRight.y;
    const float fZ = vecLeft.z - vecRight.z;
    return fX * fX + fY * fY + fZ * fZ;
}

float GetDoorMatrixDifferenceSq(const DoorGuard& guard)
{
    if (!guard.m_pDoor)
    {
        return FLT_MAX;
    }

    CMatrixLink* pMatrix = guard.m_pDoor->GetMatrix();
    if (!pMatrix)
    {
        return FLT_MAX;
    }

    return VectorDifferenceSq(pMatrix->right, guard.m_matrixClosed.right) +
        VectorDifferenceSq(pMatrix->up, guard.m_matrixClosed.up) +
        VectorDifferenceSq(pMatrix->at, guard.m_matrixClosed.at) +
        VectorDifferenceSq(pMatrix->pos, guard.m_matrixClosed.pos);
}

bool IsRealDoorClosed(const DoorGuard& guard)
{
    if (!guard.m_pDoor || guard.m_pDoor->m_nType != ENTITY_TYPE_OBJECT)
    {
        return false;
    }

    CObject* pDoor = static_cast<CObject*>(guard.m_pDoor);
    return pDoor->m_nPhysicalFlags.bCollidable && GetDoorMatrixDifferenceSq(guard) <= CLOSED_MATRIX_DIFFERENCE_SQ;
}

CVector GetEntrancePosition(CEntryExit* pEntryExit)
{
    return CVector((pEntryExit->m_recEntrance.left + pEntryExit->m_recEntrance.right) * 0.5f,
        (pEntryExit->m_recEntrance.bottom + pEntryExit->m_recEntrance.top) * 0.5f, pEntryExit->m_fEntranceZ);
}

void SetRemotePrimaryTask(CPlayerPed* pPlayerPed, CTask* pTask)
{
    CPad* pPad = CPad::GetPad(0);
    uint16_t localDisablePlayerControls = pPad->DisablePlayerControls;
    pPlayerPed->m_pIntelligence->m_TaskMgr.SetTask(pTask, TASK_PRIMARY_PRIMARY, false);
    pPad->DisablePlayerControls = localDisablePlayerControls;
}

DoorGuardMap::iterator DestroyDoorGuard(DoorGuardMap::iterator it)
{
    DoorGuard& guard = it->second;

    if (guard.m_pBlocker)
    {
        guard.m_pBlocker->m_bUsesCollision = false;
        CWorld::Remove(guard.m_pBlocker);
        CWorld::RemoveReferencesToDeletedObject(guard.m_pBlocker);
        delete guard.m_pBlocker;
        guard.m_pBlocker = nullptr;
    }

    if (guard.m_pDoor)
    {
        guard.m_pDoor->CleanUpOldReference(&guard.m_pDoor);
    }

    return g_doorGuards.erase(it);
}

DoorGuard* AcquireDoorGuard(CObject* pDoor)
{
    auto existing = g_doorGuards.find(pDoor);
    if (existing != g_doorGuards.end())
    {
        DoorGuard& guard = existing->second;
        guard.m_nNoLeaseSince = 0;
        return &guard;
    }

    CBuilding* pBlocker = nullptr;
    DoorGuardMap::iterator it;
    bool bInserted = false;
    try
    {
        pBlocker = new CBuilding();
        auto result = g_doorGuards.try_emplace(pDoor);
        it = result.first;
        bInserted = result.second;
    }
    catch (...)
    {
        delete pBlocker;
        return nullptr;
    }

    DoorGuard& guard = it->second;
    if (!bInserted)
    {
        delete pBlocker;
        guard.m_nNoLeaseSince = 0;
        return &guard;
    }

    guard.m_pDoor = pDoor;
    pDoor->RegisterReference(&guard.m_pDoor);
    guard.m_pBlocker = pBlocker;
    guard.m_matrixClosed = *pDoor->GetMatrix();

    pBlocker->SetModelIndexNoCreate(pDoor->m_nModelIndex);
    pBlocker->SetMatrix(guard.m_matrixClosed);
    pBlocker->m_nAreaCode = pDoor->m_nAreaCode;
    pBlocker->m_bUsesCollision = true;
    pBlocker->m_bIsStatic = true;
    pBlocker->m_bIsVisible = false;
    pBlocker->m_bStreamingDontDelete = true;
    pBlocker->m_bDontStream = true;
    pBlocker->m_bDontCastShadowsOn = true;
    pBlocker->m_bHasPreRenderEffects = false;
    CWorld::Add(pBlocker);

    return &guard;
}

void DetachParticipant(CollisionParticipant& participant)
{
    if (!participant.m_pGuard)
    {
        return;
    }

    DoorGuard* pGuard = participant.m_pGuard;

    if (participant.m_pPed && participant.m_pPed->m_nType == ENTITY_TYPE_PED)
    {
        CPed* pPed = static_cast<CPed*>(participant.m_pPed);
        if (pPed->m_pEntityIgnoredCollision == pGuard->m_pBlocker)
        {
            pPed->m_pEntityIgnoredCollision = participant.m_pPreviousIgnoredCollision;
        }
    }

    if (participant.m_pPreviousIgnoredCollision)
    {
        participant.m_pPreviousIgnoredCollision->CleanUpOldReference(&participant.m_pPreviousIgnoredCollision);
    }
    if (participant.m_pPed)
    {
        participant.m_pPed->CleanUpOldReference(&participant.m_pPed);
    }

    if (pGuard->m_nActiveTransitions > 0)
    {
        --pGuard->m_nActiveTransitions;
    }
    if (pGuard->m_nActiveTransitions == 0)
    {
        pGuard->m_nNoLeaseSince = GetTickCount();
    }

    participant = {};
}

void AttachParticipant(CollisionParticipant& participant, DoorGuard* pGuard, CPed* pPed)
{
    if (participant.m_pGuard)
    {
        DetachParticipant(participant);
    }

    participant.m_pPed = pPed;
    pPed->RegisterReference(&participant.m_pPed);
    participant.m_pPreviousIgnoredCollision = pPed->m_pEntityIgnoredCollision;
    if (participant.m_pPreviousIgnoredCollision == pGuard->m_pBlocker)
    {
        participant.m_pPreviousIgnoredCollision = nullptr;
    }
    else if (participant.m_pPreviousIgnoredCollision)
    {
        participant.m_pPreviousIgnoredCollision->RegisterReference(&participant.m_pPreviousIgnoredCollision);
    }

    participant.m_pGuard = pGuard;
    ++pGuard->m_nActiveTransitions;
    pGuard->m_nNoLeaseSince = 0;
    pPed->m_pEntityIgnoredCollision = pGuard->m_pBlocker;
}

bool ProcessParticipant(CollisionParticipant& participant)
{
    if (!participant.m_pGuard)
    {
        return true;
    }
    if (!participant.m_pPed || participant.m_pPed->m_nType != ENTITY_TYPE_PED)
    {
        DetachParticipant(participant);
        return false;
    }

    CPed* pPed = static_cast<CPed*>(participant.m_pPed);
    if (pPed->m_pEntityIgnoredCollision != participant.m_pGuard->m_pBlocker)
    {
        pPed->m_pEntityIgnoredCollision = participant.m_pGuard->m_pBlocker;
    }
    return true;
}

void ApplyRemoteSnapshot(CNetworkPlayer* pNetworkPlayer, const Packets::Players::EnExTransition& packet)
{
    CPlayerPed* pPlayerPed = pNetworkPlayer->m_pPed;

    if (packet.bFinished)
    {
        pPlayerPed->Teleport(packet.position, false);
    }
    else
    {
        pPlayerPed->SetPosn(packet.position);
    }
    pPlayerPed->m_fCurrentRotation = packet.currentRotation.m_angle;
    pPlayerPed->m_fAimingRotation = packet.aimingRotation.m_angle;
    pPlayerPed->SetHeading(packet.currentRotation.m_angle);
    pPlayerPed->m_nAreaCode = packet.playerAreaId;
    pPlayerPed->UpdateRwMatrix();
    pPlayerPed->m_pEnex = nullptr;
    pPlayerPed->m_vecMoveSpeed = CVector{};

    pNetworkPlayer->m_onFootSnapshotInterpolated.vecPos = packet.position;
    pNetworkPlayer->m_onFootSnapshotInterpolated.vecMoveSpeed = CVector();
    pNetworkPlayer->m_onFootSnapshotInterpolated.currentRotation = packet.currentRotation;
    pNetworkPlayer->m_onFootSnapshotInterpolated.aimingRotation = packet.aimingRotation;
}

RemoteTransitionMap::iterator ClearRemoteTransition(
    RemoteTransitionMap::iterator it, CNetworkPlayer* pNetworkPlayer)
{
    RemoteTransition& transition = it->second;
    CPlayerPed* pPlayerPed = transition.m_pPed;

    if (pNetworkPlayer && pNetworkPlayer->m_pPed == pPlayerPed)
    {
        CTaskManager& taskManager = pPlayerPed->m_pIntelligence->m_TaskMgr;
        if (taskManager.m_aPrimaryTasks[TASK_PRIMARY_PRIMARY] == transition.m_pTask)
        {
            SetRemotePrimaryTask(pPlayerPed, nullptr);
        }
    }

    DetachParticipant(transition.m_participant);
    return g_remoteTransitions.erase(it);
}

void ClearRemoteTransition(CNetworkPlayer* pNetworkPlayer)
{
    auto it = g_remoteTransitions.find(pNetworkPlayer->m_iPlayerId);
    if (it != g_remoteTransitions.end())
    {
        ClearRemoteTransition(it, pNetworkPlayer);
    }
}

void StartRemoteTransition(CNetworkPlayer* pNetworkPlayer, CEntryExit* pEntryExit, bool bUsesDoor)
{
    RemoteTransition& transition = g_remoteTransitions.try_emplace(pNetworkPlayer->m_iPlayerId).first->second;
    CPlayerPed* pPlayerPed = pNetworkPlayer->m_pPed;
    CTask* pTask = nullptr;

    if (bUsesDoor)
    {
        CEntity* pEntity = CEntryExitManager::FindNearestDoor(*pEntryExit, 10.0f);
        if (pEntity && pEntity->m_nType == ENTITY_TYPE_OBJECT)
        {
            CObject* pDoor = static_cast<CObject*>(pEntity);
            DoorGuard* pGuard = AcquireDoorGuard(pDoor);
            if (pGuard)
            {
                AttachParticipant(transition.m_participant, pGuard, pPlayerPed);
                pTask = new CTaskComplexGotoDoorAndOpen(pDoor);
            }
        }
    }

    if (!pTask)
    {
        CVector vecStart = GetEntrancePosition(pEntryExit);
        CEntryExit* pSpawnPoint = pEntryExit->m_pLink ? pEntryExit->m_pLink : pEntryExit;
        CVector vecDirection = pSpawnPoint->m_vecExitPos - vecStart;
        if (vecDirection.x * vecDirection.x + vecDirection.y * vecDirection.y +
            vecDirection.z * vecDirection.z > 0.000001f)
        {
            vecDirection.Normalise();
        }
        CVector vecEnd = vecStart + vecDirection * 4.0f;
        pTask = new CTaskComplexGotoDoorAndOpen(vecStart, vecEnd);
    }

    SetRemotePrimaryTask(pPlayerPed, pTask);
    transition.m_pPed = pPlayerPed;
    transition.m_pTask = pTask;
}

CEntryExit* FindEntryExit(int16_t rectLeft, int16_t rectBottom, uint8_t areaId)
{
    for (auto pEntryExit : CEntryExitManager::mp_poolEntryExits)
    {
        if (static_cast<int16_t>(std::floor(pEntryExit->m_recEntrance.left)) == rectLeft &&
            static_cast<int16_t>(std::floor(pEntryExit->m_recEntrance.bottom)) == rectBottom &&
            pEntryExit->m_nArea == areaId)
        {
            return pEntryExit;
        }
    }

    return nullptr;
}

void BuildAndSendTransition(CPed* pPed, Packets::Players::EnExTransition& packet)
{
    packet.position = pPed->GetPosition();
    packet.currentRotation = pPed->m_fCurrentRotation;
    packet.aimingRotation = pPed->m_fAimingRotation;
    packet.playerAreaId = pPed->m_nAreaCode;
    GetPacketFactory().Send(packet);
}

void ProcessDoorGuards()
{
    for (auto it = g_doorGuards.begin(); it != g_doorGuards.end();)
    {
        DoorGuard& guard = it->second;
        if (guard.m_nActiveTransitions > 0)
        {
            ++it;
            continue;
        }
        if (!guard.m_pDoor)
        {
            it = DestroyDoorGuard(it);
            continue;
        }
        if (IsRealDoorClosed(guard))
        {
            it = DestroyDoorGuard(it);
            continue;
        }

        if (guard.m_nNoLeaseSince != 0 && GetTickCount() - guard.m_nNoLeaseSince >= DOOR_CLOSE_TIMEOUT_MS)
        {
            it = DestroyDoorGuard(it);
            continue;
        }
        ++it;
    }
}

void ClearAllState()
{
    for (auto it = g_remoteTransitions.begin(); it != g_remoteTransitions.end();)
    {
        CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(it->first);
        it = ClearRemoteTransition(it, pNetworkPlayer);
    }

    DetachParticipant(g_localParticipant);

    for (auto it = g_doorGuards.begin(); it != g_doorGuards.end();)
    {
        it = DestroyDoorGuard(it);
    }
}
}  // namespace

void CEntryExitTransitionSync::OnTransitionStarted(CEntryExit* pEntryExit, CPed* pPed)
{
    if (!CNetwork::m_bAuthenticated || pPed != FindPlayerPed(0) || pPed->m_nPedFlags.bInVehicle ||
        pEntryExit->m_nFlags.bUnknownPairing || pEntryExit->m_nFlags.bFoodDateFlag)
    {
        return;
    }

    DetachParticipant(g_localParticipant);
    ms_pLocalAnimatedTransition = pEntryExit;

    CObject* pDoor = CEntryExit::ms_pDoor;
    if (pDoor)
    {
        DoorGuard* pGuard = AcquireDoorGuard(pDoor);
        if (pGuard)
        {
            AttachParticipant(g_localParticipant, pGuard, pPed);
        }
    }

    Packets::Players::EnExTransition packet{};
    packet.enexAreaId = pEntryExit->m_nArea;
    packet.rectLeft = static_cast<int16_t>(std::floor(pEntryExit->m_recEntrance.left));
    packet.rectBottom = static_cast<int16_t>(std::floor(pEntryExit->m_recEntrance.bottom));
    packet.bUsesDoor = pDoor != nullptr;

    BuildAndSendTransition(pPed, packet);
}

void CEntryExitTransitionSync::OnTransitionFinished(CEntryExit* pEntryExit, CPed* pPed)
{
    if (pEntryExit != ms_pLocalAnimatedTransition)
    {
        return;
    }

    if (CNetwork::m_bAuthenticated && pPed == FindPlayerPed(0))
    {
        Packets::Players::EnExTransition packet{};
        packet.bFinished = true;
        BuildAndSendTransition(pPed, packet);
    }

    DetachParticipant(g_localParticipant);
    ms_pLocalAnimatedTransition = nullptr;
}

void CEntryExitTransitionSync::Receive(const Packets::Players::EnExTransition& packet)
{
    CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(packet.playerid);
    if (!pNetworkPlayer || !pNetworkPlayer->m_pPed)
    {
        return;
    }

    CPlayerPed* pPlayerPed = pNetworkPlayer->m_pPed;
    CEntryExit* pEntryExit = nullptr;
    if (!packet.bFinished)
    {
        pEntryExit = FindEntryExit(packet.rectLeft, packet.rectBottom, packet.enexAreaId);
        if (!pEntryExit || pPlayerPed->m_nPedFlags.bInVehicle)
        {
            return;
        }
    }

    ClearRemoteTransition(pNetworkPlayer);
    ApplyRemoteSnapshot(pNetworkPlayer, packet);

    if (!packet.bFinished)
    {
        StartRemoteTransition(pNetworkPlayer, pEntryExit, packet.bUsesDoor);
    }
}

void CEntryExitTransitionSync::Process()
{
    if (!CNetwork::m_bAuthenticated)
    {
        if (!g_remoteTransitions.empty() || g_localParticipant.m_pGuard || !g_doorGuards.empty())
        {
            ClearAllState();
        }
        ms_pLocalAnimatedTransition = nullptr;
        return;
    }

    if (g_localParticipant.m_pGuard)
    {
        ProcessParticipant(g_localParticipant);
    }

    for (auto it = g_remoteTransitions.begin(); it != g_remoteTransitions.end();)
    {
        CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(it->first);
        RemoteTransition& transition = it->second;
        CPlayerPed* pTransitionPed = transition.m_pPed;

        if (!pNetworkPlayer || !pNetworkPlayer->m_pPed || pNetworkPlayer->m_pPed != pTransitionPed)
        {
            it = ClearRemoteTransition(it, pNetworkPlayer);
            continue;
        }
        if (pNetworkPlayer->m_pPed->m_nPedFlags.bInVehicle)
        {
            it = ClearRemoteTransition(it, pNetworkPlayer);
            continue;
        }
        ProcessParticipant(transition.m_participant);

        CTask* pPrimaryTask = pTransitionPed->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[TASK_PRIMARY_PRIMARY];
        if (transition.m_pTask && pPrimaryTask != transition.m_pTask && !pPrimaryTask)
        {
            transition.m_pTask = new CTaskSimpleUninterruptable();
            SetRemotePrimaryTask(pTransitionPed, transition.m_pTask);
        }
        ++it;
    }

    ProcessDoorGuards();
}

void CEntryExitTransitionSync::Shutdown()
{
    if (!g_remoteTransitions.empty() || g_localParticipant.m_pGuard || !g_doorGuards.empty())
    {
        ClearAllState();
    }
    ms_pLocalAnimatedTransition = nullptr;
}
