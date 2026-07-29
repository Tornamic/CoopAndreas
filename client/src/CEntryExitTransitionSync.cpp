#include "stdafx.h"
#include "CEntryExitTransitionSync.h"
#include "CEntryExitManager.h"
#include <game_sa/CTaskComplexGotoDoorAndOpen.h>
#include <game_sa/CTaskSimpleUninterruptable.h>

namespace
{
struct RemoteTransition
{
    CPlayerPed* ped = nullptr;
    CTask* task = nullptr;
};

std::unordered_map<int, RemoteTransition> g_remoteTransitions;

CVector GetEntrancePosition(CEntryExit* entryExit)
{
    return CVector((entryExit->m_recEntrance.left + entryExit->m_recEntrance.right) * 0.5f,
        (entryExit->m_recEntrance.bottom + entryExit->m_recEntrance.top) * 0.5f, entryExit->m_fEntranceZ);
}

void SetRemotePrimaryTask(CPlayerPed* ped, CTask* task)
{
    CPad* localPad = CPad::GetPad(0);
    uint16_t localDisablePlayerControls = localPad->DisablePlayerControls;
    ped->m_pIntelligence->m_TaskMgr.SetTask(task, TASK_PRIMARY_PRIMARY, false);
    localPad->DisablePlayerControls = localDisablePlayerControls;
}

void ApplyRemoteSnapshot(CNetworkPlayer* networkPlayer, const Packets::Players::EnExTransition& packet)
{
    CPlayerPed* ped = networkPlayer->m_pPed;
    if (packet.bFinished)
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
    ped->m_pEnex = nullptr;
    ped->m_vecMoveSpeed = CVector{};

    networkPlayer->m_onFootSnapshotInterpolated.vecPos = packet.position;
    networkPlayer->m_onFootSnapshotInterpolated.vecMoveSpeed = CVector{};
    networkPlayer->m_onFootSnapshotInterpolated.currentRotation = packet.currentRotation;
    networkPlayer->m_onFootSnapshotInterpolated.aimingRotation = packet.aimingRotation;
}

void ClearRemoteTransition(CNetworkPlayer* networkPlayer)
{
    auto it = g_remoteTransitions.find(networkPlayer->m_iPlayerId);
    if (it == g_remoteTransitions.end())
    {
        return;
    }

    RemoteTransition& transition = it->second;
    if (networkPlayer->m_pPed == transition.ped)
    {
        CTaskManager& taskManager = transition.ped->m_pIntelligence->m_TaskMgr;
        if (taskManager.m_aPrimaryTasks[TASK_PRIMARY_PRIMARY] == transition.task)
        {
            SetRemotePrimaryTask(transition.ped, nullptr);
        }
    }

    g_remoteTransitions.erase(it);
}

void StartRemoteTransition(CNetworkPlayer* networkPlayer, CEntryExit* entryExit, bool usesDoor)
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

    SetRemotePrimaryTask(ped, task);
    g_remoteTransitions[networkPlayer->m_iPlayerId] = {ped, task};
}

CEntryExit* FindEntryExit(int16_t rectLeft, int16_t rectBottom, uint8_t areaId)
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

void SendTransition(CPed* ped, Packets::Players::EnExTransition packet)
{
    packet.position = ped->GetPosition();
    packet.currentRotation = ped->m_fCurrentRotation;
    packet.aimingRotation = ped->m_fAimingRotation;
    packet.playerAreaId = ped->m_nAreaCode;
    GetPacketFactory().Send(packet);
}
}  // namespace

void CEntryExitTransitionSync::OnTransitionStarted(CEntryExit* entryExit, CPed* ped)
{
    if (!CNetwork::m_bAuthenticated || ped != FindPlayerPed(0) || ped->m_nPedFlags.bInVehicle ||
        entryExit->m_nFlags.bUnknownPairing || entryExit->m_nFlags.bFoodDateFlag)
    {
        return;
    }

    ms_pLocalAnimatedTransition = entryExit;

    Packets::Players::EnExTransition packet{};
    packet.enexAreaId = entryExit->m_nArea;
    packet.rectLeft = static_cast<int16_t>(std::floor(entryExit->m_recEntrance.left));
    packet.rectBottom = static_cast<int16_t>(std::floor(entryExit->m_recEntrance.bottom));
    packet.bUsesDoor = CEntryExit::ms_pDoor != nullptr;
    SendTransition(ped, packet);
}

void CEntryExitTransitionSync::OnTransitionFinished(CEntryExit* entryExit, CPed* ped)
{
    if (entryExit != ms_pLocalAnimatedTransition)
    {
        return;
    }

    if (CNetwork::m_bAuthenticated && ped == FindPlayerPed(0))
    {
        Packets::Players::EnExTransition packet{};
        packet.bFinished = true;
        SendTransition(ped, packet);
    }

    ms_pLocalAnimatedTransition = nullptr;
}

void CEntryExitTransitionSync::Receive(const Packets::Players::EnExTransition& packet)
{
    CNetworkPlayer* networkPlayer = CNetworkPlayerManager::GetPlayer(packet.playerid);
    if (!networkPlayer || !networkPlayer->m_pPed)
    {
        return;
    }

    CPlayerPed* ped = networkPlayer->m_pPed;
    CEntryExit* entryExit = nullptr;
    if (!packet.bFinished)
    {
        entryExit = FindEntryExit(packet.rectLeft, packet.rectBottom, packet.enexAreaId);
        if (!entryExit || ped->m_nPedFlags.bInVehicle)
        {
            return;
        }
    }

    ClearRemoteTransition(networkPlayer);
    ApplyRemoteSnapshot(networkPlayer, packet);

    if (!packet.bFinished)
    {
        StartRemoteTransition(networkPlayer, entryExit, packet.bUsesDoor);
    }
}

void CEntryExitTransitionSync::Process()
{
    for (auto it = g_remoteTransitions.begin(); it != g_remoteTransitions.end();)
    {
        auto current = it++;
        CNetworkPlayer* networkPlayer = CNetworkPlayerManager::GetPlayer(current->first);
        RemoteTransition& transition = current->second;
        if (!networkPlayer || !networkPlayer->m_pPed || networkPlayer->m_pPed != transition.ped)
        {
            g_remoteTransitions.erase(current);
            continue;
        }

        if (networkPlayer->m_pPed->m_nPedFlags.bInVehicle)
        {
            ClearRemoteTransition(networkPlayer);
            continue;
        }

        CTask* primaryTask = transition.ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[TASK_PRIMARY_PRIMARY];
        if (transition.task && primaryTask != transition.task && !primaryTask)
        {
            transition.task = new CTaskSimpleUninterruptable();
            SetRemotePrimaryTask(transition.ped, transition.task);
        }
    }
}
