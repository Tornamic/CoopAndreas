#include "CEntryExitMarkerSync.h"
#include "CNetworkStaticBlip.h"
#include "network/packet_types.h"
#include "network/packets/system.h"
#include "stdafx.h"
#include <CWeatherSync.h>
#include <game_sa/CTagManager.h>

PACKET_HANDLER(ePacketType::PLAYER_CONNECTED, Packets::System::PlayerConnected* pPlayerConnected)
{
    CNetworkPlayer* pNetworkPlayer =
        new CNetworkPlayer(pPlayerConnected->payload.playerid, CVector(2246.506f, -1259.552f, 23.9531f));
    CNetworkPlayerManager::Add(pNetworkPlayer);

    strcpy_s(pNetworkPlayer->m_Name, pPlayerConnected->payload.name);

    pNetworkPlayer->m_bHasBeenConnectedBeforeMe = pPlayerConnected->payload.isAlreadyConnected;

    if (!pNetworkPlayer->m_bHasBeenConnectedBeforeMe)
    {
        CChat::AddMessage("[Player] " + pNetworkPlayer->GetName() + " connected");
    }
    // TODO REFACTOR
    if (CLocalPlayer::m_bIsHost)
    {
        CNetworkStaticBlip::ms_bNeedToSendAfterThisFrame = true;
        CEntryExitMarkerSync::ms_bUpdateAfterProcessingThisFrame = true;
        CTagSync::SyncCurrentState();
    }
}

PACKET_HANDLER(ePacketType::PLAYER_DISCONNECTED, Packets::System::PlayerDisconnected* pPlayerDisconnected)
{
    if (pPlayerDisconnected->payload.playerid == -1)
    {
        if (pPlayerDisconnected->payload.reason == Packets::System::PlayerDisconnected::DISCONNECTION_REASON_VERSION_MISMATCH)
        {
            char buffer[23];
            semver_t expected;
            semver_unpack(pPlayerDisconnected->payload.version, &expected);
            semver_to_string(&expected, buffer, sizeof buffer);
            buffer[22] = '\0';
            CChat::AddMessage("{cecedb}[Network] Version mismatch, server: %s client: %s", buffer, COOPANDREAS_VERSION);
        }
        else if (pPlayerDisconnected->payload.reason == Packets::System::PlayerDisconnected::DISCONNECTION_REASON_NAME_TAKEN)
        {
            CChat::AddMessage("{cecedb}[Network] The nickname '%s' is already taken.", CLocalPlayer::m_Name);
        }
        CNetwork::Disconnect();
        return;
    }

    CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(pPlayerDisconnected->payload.playerid);
    if (player != nullptr)
    {
        CChat::AddMessage("[Player] " + std::string(player->GetName()) + " disconnected");
        CNetworkPlayerManager::Remove(player);
        delete player;
    }
}

PACKET_HANDLER(ePacketType::PLAYER_HANDSHAKE, Packets::System::PlayerHandshake* pPlayerHandshake)
{
    CNetworkPlayerManager::m_nMyId = pPlayerHandshake->yourid;
    CLocalPlayer::m_nPing = 0;
    CNetwork::m_bAuthenticated = true;
    CPatch::RevertTemporaryPatches();
    logger::info("Authenticated, playerid %d", pPlayerHandshake->yourid);
}

PACKET_HANDLER(ePacketType::PLAYER_PING, Packets::System::PlayerPing* pPlayerPing)
{
    int nExpectedPlayerCount = static_cast<int>(CNetworkPlayerManager::m_pPlayers.size()) + 1;
    if (pPlayerPing->playerCount != nExpectedPlayerCount)
    {
        logger::warn("Player ping count mismatch: expected %d, received %d", nExpectedPlayerCount,
            pPlayerPing->playerCount);
        return;
    }

    int nPingIndex = 0;
    for (int nPlayerId = 0; nPlayerId < Config::MAX_SERVER_PLAYERS; nPlayerId++)
    {
        if (nPlayerId == CNetworkPlayerManager::m_nMyId)
        {
            CLocalPlayer::m_nPing = pPlayerPing->ping[nPingIndex++];
            continue;
        }

        CNetworkPlayer* pNetworkPlayer = CNetworkPlayerManager::GetPlayer(nPlayerId);
        if (pNetworkPlayer != nullptr)
        {
            pNetworkPlayer->m_nPing = pPlayerPing->ping[nPingIndex++];
        }
    }
}

PACKET_HANDLER(ePacketType::PLAYER_ASSIGN_HOST, Packets::System::PlayerAssignHost* pPlayerAssignHost)
{
    if (pPlayerAssignHost->playerid == CNetworkPlayerManager::m_nMyId)
    {
        CLocalPlayer::m_bIsHost = true;

        CPatch::RevertTemporaryPatchesForHost(); // TODO is this needed?

        CNetworkPedManager::AssignHost();
        CWeatherSync::SyncCurrentState();
        CTagSync::SyncCurrentState();
        CChat::AddMessage("[Player] You are the host now");
        return;
    }
    
    for (auto pNetworkPlayer : CNetworkPlayerManager::m_pPlayers)
    {
        if (pNetworkPlayer->m_iPlayerId == pPlayerAssignHost->playerid)
        {
            CChat::AddMessage("[Player] " + pNetworkPlayer->GetName() + " is the host now");
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
    CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(pChatMessage->playerid);

    if (player)
    {
        CChat::SendPlayerMessage(player->GetName().c_str(), player->m_iPlayerId, pChatMessage->message);
    }
}
