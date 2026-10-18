#include "CEntryExitMarkerSync.h"
#include "CNetworkStaticBlip.h"
#include "network/packet_types.h"
#include "network/packets/system.h"
#include "stdafx.h"
#include <CWeatherSync.h>
#include <CMoonSync.h>
#include <game_sa/CTagManager.h>

PACKET_HANDLER(ePacketType::PLAYER_CONNECTED, Packets::System::PlayerConnected* pPlayerConnected)
{
    CNetworkPlayer* pNetworkPlayer =
        new CNetworkPlayer(pPlayerConnected->payload.GetPlayerId(), CVector(2246.506f, -1259.552f, 23.9531f));
    CNetworkPlayerManager::Add(pNetworkPlayer);

    strcpy_s(pNetworkPlayer->m_Name, pPlayerConnected->payload.name);

    pNetworkPlayer->m_bHasBeenConnectedBeforeMe = pPlayerConnected->payload.isAlreadyConnected;

    if (!pNetworkPlayer->m_bHasBeenConnectedBeforeMe)
    {
        CChat::AddMessage("{c7a6ff}[Player] {d6dce8}" + pNetworkPlayer->GetName() + " connected");
    }
    // TODO REFACTOR
    if (CLocalPlayer::m_bIsHost)
    {
        CNetworkStaticBlip::ms_bNeedToSendAfterThisFrame = true;
        CEntryExitMarkerSync::ms_bUpdateAfterProcessingThisFrame = true;
        CTagSync::SyncCurrentState();
        CMoonSync::SyncCurrentState();
    }
}

PACKET_HANDLER(ePacketType::PLAYER_DISCONNECTED, Packets::System::PlayerDisconnected* pPlayerDisconnected)
{
    if (pPlayerDisconnected->payload.playerid == -1)
    {
        if (pPlayerDisconnected->payload.reason ==
            Packets::System::PlayerDisconnected::DISCONNECTION_REASON_VERSION_MISMATCH)
        {
            char buffer[23];
            semver_t expected;
            semver_unpack(pPlayerDisconnected->payload.version, &expected);
            semver_to_string(&expected, buffer, sizeof buffer);
            buffer[22] = '\0';
            CChat::AddMessage(
                "{63c7e8}[Network] {e8b86d}Version mismatch, server: %s client: %s", buffer, COOPANDREAS_VERSION);
        }
        else if (pPlayerDisconnected->payload.reason ==
                 Packets::System::PlayerDisconnected::DISCONNECTION_REASON_NAME_TAKEN)
        {
            CChat::AddMessage("{63c7e8}[Network] {e8b86d}Nickname '%s' is already taken.", CLocalPlayer::m_Name);
        }
        CNetwork::Disconnect();
        return;
    }

    CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(pPlayerDisconnected->payload.GetPlayerId());
    if (player != nullptr)
    {
        CChat::AddMessage("{c7a6ff}[Player] {d6dce8}" + std::string(player->GetName()) + " disconnected");
        CNetworkPlayerManager::Remove(player);
        delete player;
    }
}

PACKET_HANDLER(ePacketType::PLAYER_HANDSHAKE, Packets::System::PlayerHandshake* pPlayerHandshake)
{
    if (!pPlayerHandshake->yourid.valid())
        return;

    CNetworkPlayerManager::m_localPlayerId = pPlayerHandshake->yourid;

    CNetworkVehicleManager::ResetIdAllocator();
    CNetworkPedManager::ResetIdAllocator();

    CNetwork::m_bAuthenticated = true;
    CPatch::RevertTemporaryPatches();

    // stats are sent only when they change, so the server would not know the ones we already have
    CStatsSync::NotifyChanged();
    logger::info("Authenticated, playerid %s", to_string(pPlayerHandshake->yourid).c_str());
}

PACKET_HANDLER(ePacketType::RTT_BROADCAST, Packets::System::RTTBroadcast* pRTTBroadcast)
{
    for (int nRTTIndex = 0; nRTTIndex < pRTTBroadcast->playerCount; nRTTIndex++)
    {
        Packets::System::RTTBroadcast::SPlayerRTT& playerRTT = pRTTBroadcast->rtts[nRTTIndex];
        if (playerRTT.playerid == CNetworkPlayerManager::m_localPlayerId)
        {
            continue;
        }

        CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(playerRTT.playerid);
        if (pNetworkPlayer != nullptr)
        {
            pNetworkPlayer->m_nRTT = playerRTT.rtt;
        }
    }
}

PACKET_HANDLER(ePacketType::PLAYER_ASSIGN_HOST, Packets::System::PlayerAssignHost* pPlayerAssignHost)
{
    if (pPlayerAssignHost->playerid == CNetworkPlayerManager::m_localPlayerId)
    {
        CLocalPlayer::m_bIsHost = true;

        CPatch::RevertTemporaryPatchesForHost();  // TODO is this needed?

        CNetworkPedManager::AssignHost();
        CWeatherSync::SyncCurrentState();
        CTagSync::SyncCurrentState();
        CChat::AddMessage("{c7a6ff}[Player] {d6dce8}You are the host now");
        return;
    }

    for (auto pNetworkPlayer : CNetworkPlayerManager::m_pPlayers)
    {
        if (pNetworkPlayer->m_playerId == pPlayerAssignHost->playerid)
        {
            CChat::AddMessage("{c7a6ff}[Player] {d6dce8}" + pNetworkPlayer->GetName() + " is the host now");
            pNetworkPlayer->m_bIsHost = true;
        }
        else
        {
            pNetworkPlayer->m_bIsHost = false;
        }
    }

    CLocalPlayer::m_bIsHost = false;
}

PACKET_HANDLER(ePacketType::PLAYER_CHAT_MESSAGE, Packets::System::ChatMessage* pChatMessage)
{
    CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(pChatMessage->senderPlayerid);

    if (player)
    {
        CChat::AddPlayerChatMessage(player->GetName().c_str(), pChatMessage->message);
    }
}
