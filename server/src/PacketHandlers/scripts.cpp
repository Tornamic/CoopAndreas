#include "network/packets/scripts.h"
#include "network/packet_types.h"
#include "stdafx.h"

PACKET_HANDLER(ePacketType::ON_MISSION_FLAG_SYNC, Packets::Scripts::OnMissionFlagSync* pOnMissionFlagSync,
    CNetworkPlayer* pNetworkPlayer)
{
    if (pNetworkPlayer->m_bIsHost)
    {
        GetPacketFactory().SendToAll(*pOnMissionFlagSync, pNetworkPlayer);
    }
}

PACKET_HANDLER(ePacketType::ENEX_SYNC, Packets::Scripts::EnExSync* pEnExSync, CNetworkPlayer* pNetworkPlayer)
{
    if (pNetworkPlayer->m_bIsHost)
    {
        Packets::Scripts::g_lastEnExData = *pEnExSync;
        Packets::Scripts::g_lastEnExData.serverTime = 0;  // force recalculation

        Packets::Scripts::g_pLastEnExPlayerOwner = pNetworkPlayer;

        GetPacketFactory().SendToAll(*pEnExSync, pNetworkPlayer);
    }
}

PACKET_HANDLER(
    ePacketType::ADD_MESSAGE_GXT, Packets::Scripts::AddMessageGXT* pAddMessageGXT, CNetworkPlayer* pNetworkPlayer)
{
    if (pNetworkPlayer->m_bIsHost)
    {
        if (auto pTargetPlayer = CNetworkPlayerManager::GetPlayer(pAddMessageGXT->forWhoPlayerId))
        {
            GetPacketFactory().Send(*pAddMessageGXT, pTargetPlayer);
        }
    }
}

PACKET_HANDLER(ePacketType::REMOVE_MESSAGE_GXT, Packets::Scripts::RemoveMessageGXT* pRemoveMessageGXT,
    CNetworkPlayer* pNetworkPlayer)
{
    if (pNetworkPlayer->m_bIsHost)
    {
        if (auto pTargetPlayer = CNetworkPlayerManager::GetPlayer(pRemoveMessageGXT->forWhoPlayerId))
        {
            GetPacketFactory().Send(*pRemoveMessageGXT, pTargetPlayer);
        }
    }
}

#if 0  // controlled with SCM
PACKET_HANDLER(ePacketType::START_CUTSCENE, Packets::Scripts::StartCutscene* pStartCutscene, CNetworkPlayer* pNetworkPlayer)
{
    if (pNetworkPlayer->m_bIsHost)
    {
        GetPacketFactory().SendToAll(*pStartCutscene, pNetworkPlayer);
    }
}

PACKET_HANDLER(ePacketType::SKIP_CUTSCENE, Packets::Scripts::SkipCutscene* pSkipCutscene, CNetworkPlayer* pNetworkPlayer)
{
    pSkipCutscene->playerid = pNetworkPlayer->m_iPlayerId;
    GetPacketFactory().SendToAll(*pSkipCutscene, pNetworkPlayer);
}
#endif

PACKET_HANDLER(ePacketType::PLAY_MISSION_AUDIO, Packets::Scripts::PlayMissionAudio* pPlayMissionAudio,
    CNetworkPlayer* pNetworkPlayer)
{
    if (pNetworkPlayer->m_bIsHost)
    {
        GetPacketFactory().SendToAll(*pPlayMissionAudio, pNetworkPlayer);
    }
}

PACKET_HANDLER(ePacketType::TELEPORT_PLAYER_SCRIPTED, Packets::Scripts::TeleportPlayerScripted* pTeleportPlayerScripted,
    CNetworkPlayer* pNetworkPlayer)
{
    if (pNetworkPlayer->m_bIsHost)
    {
        if (auto pTargetPlayer = CNetworkPlayerManager::GetPlayer(pTeleportPlayerScripted->playerid))
        {
            GetPacketFactory().Send(*pTeleportPlayerScripted, pTargetPlayer);
        }
    }
}

PACKET_HANDLER(ePacketType::OPCODE_SYNC, Packets::Scripts::OpCodeSync* pOpCodeSync, CNetworkPlayer* pNetworkPlayer)
{
    GetPacketFactory().SendToAll(*pOpCodeSync, pNetworkPlayer);
}

PACKET_HANDLER(ePacketType::PERFORM_TASK_SEQUENCE, Packets::Scripts::PerformTaskSequence* pPerformTaskSequence, CNetworkPlayer* pNetworkPlayer)
{
    *(int*)&pPerformTaskSequence->buffer[0] = pNetworkPlayer->m_iPlayerId;
    GetPacketFactory().SendToAll(*pPerformTaskSequence, pNetworkPlayer);
}
