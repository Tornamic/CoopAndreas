#include "stdafx.h"
#include "CEntryExitTransitionSync.h"
#include "CEntryExitManager.h"
#include <game_sa/CTaskComplexGotoDoorAndOpen.h>
#include <game_sa/CTaskSimpleUninterruptable.h>

namespace
{
struct RemoteTransition
{
    CTask* task = nullptr;
    CTask* holdTask = nullptr;
};

std::unordered_map<int, RemoteTransition> g_remoteTransitions;

CVector GetEntrancePosition(CEntryExit* entryExit)
{
    return CVector((entryExit->m_recEntrance.left + entryExit->m_recEntrance.right) * 0.5f,
        (entryExit->m_recEntrance.bottom + entryExit->m_recEntrance.top) * 0.5f, entryExit->m_fEntranceZ);
}

void ApplyPedTransform(CPlayerPed* ped, const Packets::Scripts::EnExTransition& packet, bool teleport)
{
    if (teleport)
    {
        ped->Teleport(packet.position, false);
    }
    else
    {
        ped->SetPosn(packet.position);
    }
    ped->m_fCurrentRotation = packet.currentRotation.m_angle;
    ped->m_fAimingRotation = packet.aimingRotation.m_angle;
    ped->SetHeading(packet.currentRotation.m_angle);
    ped->m_nAreaCode = packet.playerAreaId;
    ped->UpdateRwMatrix();
}

void ClearRemoteTransition(CNetworkPlayer* networkPlayer)
{
    auto it = g_remoteTransitions.find(networkPlayer->m_iPlayerId);
    if (it == g_remoteTransitions.end())
    {
        return;
    }

    CTaskManager& taskManager = networkPlayer->m_pPed->m_pIntelligence->m_TaskMgr;
    CTask* primaryTask = taskManager.m_aPrimaryTasks[TASK_PRIMARY_PRIMARY];
    bool ownsPrimaryTask = (it->second.task && primaryTask == it->second.task) ||
        (it->second.holdTask && primaryTask == it->second.holdTask);
    if (ownsPrimaryTask)
    {
        CPad* pad = CPad::GetPad(0);
        uint16_t localDisablePlayerControls = pad->DisablePlayerControls;
        taskManager.SetTask(nullptr, TASK_PRIMARY_PRIMARY, false);
        pad->DisablePlayerControls = localDisablePlayerControls;
    }

    g_remoteTransitions.erase(it);
}

void InstallRemoteHoldTask(CNetworkPlayer* networkPlayer, RemoteTransition& transition)
{
    CPlayerPed* ped = networkPlayer->m_pPed;
    CPad* localPad = CPad::GetPad(0);
    uint16_t localDisablePlayerControls = localPad->DisablePlayerControls;
    auto* holdTask = new CTaskSimpleUninterruptable();
    ped->m_pIntelligence->m_TaskMgr.SetTask(holdTask, TASK_PRIMARY_PRIMARY, false);
    localPad->DisablePlayerControls = localDisablePlayerControls;
    transition.holdTask = holdTask;
}

void StartRemoteTransition(
    CNetworkPlayer* networkPlayer, CEntryExit* entryExit, bool usesDoor, RemoteTransition& transition)
{
    CPlayerPed* ped = networkPlayer->m_pPed;
    CTask* task = nullptr;

    if (usesDoor)
    {
        CEntity* entity = CEntryExitManager::FindNearestDoor(*entryExit, 10.0f);
        if (entity && entity->m_nType == ENTITY_TYPE_OBJECT)
        {
            task = new CTaskComplexGotoDoorAndOpen(static_cast<CObject*>(entity));
        }
    }

    if (!task)
    {
        CVector start = GetEntrancePosition(entryExit);
        CEntryExit* spawnPoint = entryExit->m_pLink ? entryExit->m_pLink : entryExit;
        CVector direction = spawnPoint->m_vecExitPos - start;
        if (direction.x * direction.x + direction.y * direction.y + direction.z * direction.z > 0.000001f)
        {
            direction.Normalise();
        }
        CVector end = start + direction * 4.0f;
        task = new CTaskComplexGotoDoorAndOpen(start, end);
    }

    transition.task = task;
    CPad* localPad = CPad::GetPad(0);
    uint16_t localDisablePlayerControls = localPad->DisablePlayerControls;
    ped->m_pIntelligence->m_TaskMgr.SetTask(task, TASK_PRIMARY_PRIMARY, false);
    localPad->DisablePlayerControls = localDisablePlayerControls;
}
}  // namespace

CEntryExit* CEntryExitTransitionSync::FindEntryExit(int16_t rectLeft, int16_t rectBottom, uint8_t areaId)
{
    for (auto entryExit : CEntryExitManager::mp_poolEntryExits)
    {
        if (static_cast<int16_t>(std::floor(entryExit->m_recEntrance.left)) == rectLeft &&
            static_cast<int16_t>(std::floor(entryExit->m_recEntrance.bottom)) == rectBottom &&
            entryExit->m_nArea == areaId)
        {
            return entryExit;
        }
    }

    return nullptr;
}

void CEntryExitTransitionSync::Send(CEntryExit* entryExit, CPed* ped, bool finished)
{
    Packets::Scripts::EnExTransition packet{};
    packet.position = ped->GetPosition();
    packet.currentRotation = ped->m_fCurrentRotation;
    packet.aimingRotation = ped->m_fAimingRotation;
    packet.playerAreaId = ped->m_nAreaCode;
    packet.bFinished = finished;
    if (finished)
    {
        packet.enexAreaId = ms_nLocalEnExAreaId;
        packet.rectLeft = ms_nLocalRectLeft;
        packet.rectBottom = ms_nLocalRectBottom;
    }
    else
    {
        packet.enexAreaId = entryExit->m_nArea;
        packet.rectLeft = static_cast<int16_t>(std::floor(entryExit->m_recEntrance.left));
        packet.rectBottom = static_cast<int16_t>(std::floor(entryExit->m_recEntrance.bottom));
        packet.bUsesDoor = CEntryExit::ms_pDoor != nullptr;
    }
    GetPacketFactory().Send(packet);
}

void CEntryExitTransitionSync::OnTransitionStarted(CEntryExit* entryExit, CPed* ped, bool started)
{
    if (!started || !CNetwork::m_bAuthenticated || ped != FindPlayerPed(0) || ped->m_nPedFlags.bInVehicle ||
        entryExit->m_nFlags.bUnknownPairing || entryExit->m_nFlags.bFoodDateFlag)
    {
        return;
    }

    ms_pLocalAnimatedTransition = entryExit;
    ms_nLocalEnExAreaId = entryExit->m_nArea;
    ms_nLocalRectLeft = static_cast<int16_t>(std::floor(entryExit->m_recEntrance.left));
    ms_nLocalRectBottom = static_cast<int16_t>(std::floor(entryExit->m_recEntrance.bottom));
    Send(entryExit, ped, false);
}

void CEntryExitTransitionSync::OnTransitionFinished(CEntryExit* entryExit, CPed* ped, bool finished)
{
    if (!finished || entryExit != ms_pLocalAnimatedTransition)
    {
        return;
    }

    if (CNetwork::m_bAuthenticated && ped == FindPlayerPed(0))
    {
        Send(nullptr, ped, true);
    }

    ms_pLocalAnimatedTransition = nullptr;
}

void CEntryExitTransitionSync::Receive(const Packets::Scripts::EnExTransition& packet)
{
    CNetworkPlayer* networkPlayer = CNetworkPlayerManager::GetPlayer(packet.playerid);
    if (!networkPlayer || !networkPlayer->m_pPed)
    {
        return;
    }

    CPlayerPed* ped = networkPlayer->m_pPed;
    if (packet.bFinished)
    {
        ClearRemoteTransition(networkPlayer);
        ApplyPedTransform(ped, packet, true);
        ped->m_pEnex = nullptr;
        ped->m_vecMoveSpeed = CVector{};
        networkPlayer->m_onFootSnapshotInterpolated.vecPos = packet.position;
        networkPlayer->m_onFootSnapshotInterpolated.vecMoveSpeed = CVector{};
        networkPlayer->m_onFootSnapshotInterpolated.currentRotation = packet.currentRotation;
        networkPlayer->m_onFootSnapshotInterpolated.aimingRotation = packet.aimingRotation;
        return;
    }

    CEntryExit* entryExit = FindEntryExit(packet.rectLeft, packet.rectBottom, packet.enexAreaId);
    if (!entryExit || ped->m_nPedFlags.bInVehicle)
    {
        return;
    }

    ClearRemoteTransition(networkPlayer);
    ApplyPedTransform(ped, packet, false);
    ped->m_pEnex = nullptr;
    ped->m_vecMoveSpeed = CVector{};
    networkPlayer->m_onFootSnapshotInterpolated.vecPos = packet.position;
    networkPlayer->m_onFootSnapshotInterpolated.vecMoveSpeed = CVector{};
    networkPlayer->m_onFootSnapshotInterpolated.currentRotation = packet.currentRotation;
    networkPlayer->m_onFootSnapshotInterpolated.aimingRotation = packet.aimingRotation;

    RemoteTransition& transition = g_remoteTransitions[networkPlayer->m_iPlayerId];
    StartRemoteTransition(networkPlayer, entryExit, packet.bUsesDoor, transition);
}

void CEntryExitTransitionSync::Process()
{
    for (auto it = g_remoteTransitions.begin(); it != g_remoteTransitions.end();)
    {
        auto current = it++;
        CNetworkPlayer* networkPlayer = CNetworkPlayerManager::GetPlayer(current->first);
        if (!networkPlayer || !networkPlayer->m_pPed)
        {
            g_remoteTransitions.erase(current);
            continue;
        }

        if (networkPlayer->m_pPed->m_nPedFlags.bInVehicle)
        {
            ClearRemoteTransition(networkPlayer);
            continue;
        }

        RemoteTransition& transition = current->second;
        CTask* primaryTask =
            networkPlayer->m_pPed->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[TASK_PRIMARY_PRIMARY];
        if (transition.task && !transition.holdTask && primaryTask != transition.task && !primaryTask)
        {
            InstallRemoteHoldTask(networkPlayer, transition);
        }
    }
}
