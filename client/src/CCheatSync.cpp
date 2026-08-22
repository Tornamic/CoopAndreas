#include "stdafx.h"
#include "CCheatSync.h"
#include "network/packets/world.h"
#include <CCheat.h>
#include <CHud.h>

static constexpr int kTotalCheats = 92;

static void (**GetGameCheatTable())()
{ return reinterpret_cast<void (**)()>(0x8A5B58); }

static bool s_applyingRemote = false;
static bool s_inited = false;
static void (*s_orig[kTotalCheats])() = {};
static uint32_t s_lastSentTick[kTotalCheats] = {};
static bool s_lastActive[kTotalCheats] = {};

static void ShowCheatActivatedMessage(const char* playerName)
{
    if (!playerName || !playerName[0])
        playerName = "Unknown";

    char buf[128];
    sprintf_s(buf, "Cheat activated by %s", playerName);
    CHud::SetHelpMessage(buf, true, false, false);
}

static std::string ResolvePlayerName(int playerid)
{
    if (playerid == CNetworkPlayerManager::m_nMyId)
        return std::string(CLocalPlayer::m_Name);

    CNetworkPlayer* player = CNetworkPlayerManager::GetPlayer(playerid);
    if (player)
        return player->GetName();

    return "Unknown";
}

template <int N>
struct CheatHook
{
    static void Fn()
    {
        if (N >= 0 && N < kTotalCheats && s_orig[N])
            s_orig[N]();

        if (!s_applyingRemote)
            CCheatSync::OnLocalCheat(static_cast<uint8_t>(N));
    }
};

template <int N>
static void InstallHooks()
{
    if (N >= 0 && N < kTotalCheats && s_orig[N])
        GetGameCheatTable()[N] = &CheatHook<N>::Fn;

    if constexpr (N > 0)
        InstallHooks<N - 1>();
}

void CCheatSync::Init()
{
    if (s_inited)
        return;
    s_inited = true;

    auto table = GetGameCheatTable();
    for (int i = 0; i < kTotalCheats; ++i)
    {
        s_orig[i] = table[i];
        s_lastSentTick[i] = 0;
        s_lastActive[i] = CCheat::m_aCheatsActive[i];
    }

    InstallHooks<kTotalCheats - 1>();
}

void CCheatSync::OnLocalCheat(uint8_t cheatId)
{
    if (!CNetwork::m_bAuthenticated || s_applyingRemote)
        return;

    if (cheatId >= kTotalCheats)
        return;

    const uint32_t now = GetTickCount();
    if (now - s_lastSentTick[cheatId] < 500)
        return;
    s_lastSentTick[cheatId] = now;

    s_lastActive[cheatId] = CCheat::m_aCheatsActive[cheatId];

    Packets::World::GameCheatSync packet{};
    packet.playerid = CNetworkPlayerManager::m_nMyId;
    packet.cheatId = cheatId;
    packet.active = CCheat::m_aCheatsActive[cheatId];
    GetPacketFactory().Send(packet);

    ShowCheatActivatedMessage(CLocalPlayer::m_Name);
}

void CCheatSync::Process()
{
    if (!s_inited)
        Init();

    if (!CNetwork::m_bAuthenticated || s_applyingRemote)
        return;

    for (int i = 0; i < kTotalCheats; ++i)
    {
        const bool active = CCheat::m_aCheatsActive[i];
        if (active == s_lastActive[i])
            continue;

        s_lastActive[i] = active;

        if (s_orig[i])
            continue;

        const uint32_t now = GetTickCount();
        if (now - s_lastSentTick[i] < 500)
            continue;
        s_lastSentTick[i] = now;

        Packets::World::GameCheatSync packet{};
        packet.playerid = CNetworkPlayerManager::m_nMyId;
        packet.cheatId = static_cast<uint8_t>(i);
        packet.active = active;
        GetPacketFactory().Send(packet);

        ShowCheatActivatedMessage(CLocalPlayer::m_Name);
    }
}

void CCheatSync::HandlePacket(Packets::World::GameCheatSync* packet)
{
    if (!packet)
        return;

    if (!s_inited)
        Init();

    if (packet->cheatId >= kTotalCheats)
        return;

    if (packet->playerid == CNetworkPlayerManager::m_nMyId)
        return;

    s_applyingRemote = true;

    if (s_orig[packet->cheatId])
        s_orig[packet->cheatId]();

    CCheat::m_aCheatsActive[packet->cheatId] = packet->active;
    s_lastActive[packet->cheatId] = packet->active;

    if (packet->active)
        CCheat::m_bHasPlayerCheated = true;

    s_applyingRemote = false;

    const std::string name = ResolvePlayerName(packet->playerid);
    ShowCheatActivatedMessage(name.c_str());
}