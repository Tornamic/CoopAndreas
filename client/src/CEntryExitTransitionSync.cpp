#include "stdafx.h"
#include "CEntryExitTransitionSync.h"
#include "CEntryExitManager.h"
#include <game_sa/CTaskComplexGotoDoorAndOpen.h>
#include <game_sa/CTaskSimpleUninterruptable.h>

namespace
{
struct RemoteTransition
{
    CPlayerPed* m_pPed = nullptr;
    CTask* m_pTask = nullptr;
};

std::unordered_map<int, RemoteTransition> g_remoteTransitions;

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

void ClearRemoteTransition(CNetworkPlayer* pNetworkPlayer)
{
    auto it = g_remoteTransitions.find(pNetworkPlayer->m_iPlayerId);
    if (it == g_remoteTransitions.end())
    {
        return;
    }

    RemoteTransition& transition = it->second;
    if (pNetworkPlayer->m_pPed == transition.m_pPed)
    {
        CTaskManager& taskManager = transition.m_pPed->m_pIntelligence->m_TaskMgr;
        if (taskManager.m_aPrimaryTasks[TASK_PRIMARY_PRIMARY] == transition.m_pTask)
        {
            SetRemotePrimaryTask(transition.m_pPed, nullptr);
        }
    }

    g_remoteTransitions.erase(it);
}

void StartRemoteTransition(CNetworkPlayer* pNetworkPlayer, CEntryExit* pEntryExit, bool bUsesDoor)
{
    CPlayerPed* pPlayerPed = pNetworkPlayer->m_pPed;
    CTask* pTask = nullptr;

    if (bUsesDoor)
    {
        CEntity* pEntity = CEntryExitManager::FindNearestDoor(*pEntryExit, 10.0f);
        if (pEntity && pEntity->m_nType == ENTITY_TYPE_OBJECT)
        {
            pTask = new CTaskComplexGotoDoorAndOpen(static_cast<CObject*>(pEntity));
        }
    }

    if (!pTask)
    {
        CVector vecStart = GetEntrancePosition(pEntryExit);
        CEntryExit* pSpawnPoint = pEntryExit->m_pLink ? pEntryExit->m_pLink : pEntryExit;
        CVector vecDirection = pSpawnPoint->m_vecExitPos - vecStart;
        if (vecDirection.x * vecDirection.x + vecDirection.y * vecDirection.y + vecDirection.z * vecDirection.z > 0.000001f)
        {
            vecDirection.Normalise();
        }
        CVector vecEnd = vecStart + vecDirection * 4.0f;
        pTask = new CTaskComplexGotoDoorAndOpen(vecStart, vecEnd);
    }

    SetRemotePrimaryTask(pPlayerPed, pTask);
    g_remoteTransitions[pNetworkPlayer->m_iPlayerId] = {pPlayerPed, pTask};
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
}  // namespace

void CEntryExitTransitionSync::OnTransitionStarted(CEntryExit* pEntryExit, CPed* pPed)
{
    if (!CNetwork::m_bAuthenticated || pPed != FindPlayerPed(0) || pPed->m_nPedFlags.bInVehicle ||
        pEntryExit->m_nFlags.bUnknownPairing || pEntryExit->m_nFlags.bFoodDateFlag)
    {
        return;
    }

    ms_pLocalAnimatedTransition = pEntryExit;

    Packets::Players::EnExTransition packet{};
    packet.enexAreaId = pEntryExit->m_nArea;
    packet.rectLeft = static_cast<int16_t>(std::floor(pEntryExit->m_recEntrance.left));
    packet.rectBottom = static_cast<int16_t>(std::floor(pEntryExit->m_recEntrance.bottom));
    packet.bUsesDoor = CEntryExit::ms_pDoor != nullptr;
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
    for (auto it = g_remoteTransitions.begin(); it != g_remoteTransitions.end();)
    {
        auto pCurrent = it++;
        CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(pCurrent->first);
        RemoteTransition& transition = pCurrent->second;
        if (!pNetworkPlayer || !pNetworkPlayer->m_pPed || pNetworkPlayer->m_pPed != transition.m_pPed)
        {
            g_remoteTransitions.erase(pCurrent);
            continue;
        }

        if (pNetworkPlayer->m_pPed->m_nPedFlags.bInVehicle)
        {
            ClearRemoteTransition(pNetworkPlayer);
            continue;
        }

        CTask* pPrimaryTask = transition.m_pPed->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[TASK_PRIMARY_PRIMARY];
        if (transition.m_pTask && pPrimaryTask != transition.m_pTask && !pPrimaryTask)
        {
            transition.m_pTask = new CTaskSimpleUninterruptable();
            SetRemotePrimaryTask(transition.m_pPed, transition.m_pTask);
        }
    }
}
