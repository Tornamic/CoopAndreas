#include "stdafx.h"
#include "CWantedSync.h"
#include <CPickups.h>

namespace
{
constexpr float PROXIMITY_RADIUS = 25.0f;
constexpr uint32_t POLICE_INVOLVEMENT_DELAY = 2000;
constexpr uint32_t EXIT_INVOLVEMENT_WINDOW = 2000;
constexpr uint32_t RESPRAY_PROXIMITY_SUSPEND = 3000;
constexpr uint32_t STATE_HEARTBEAT_RATE = 1000;
constexpr uint32_t PROCESS_RATE = 100;
constexpr uint8_t UNINITIALISED_WANTED_LEVEL = 0xFF;

uint8_t g_lastObservedWantedLevel = UNINITIALISED_WANTED_LEVEL;
bool g_lastObservedPoliceEngaged = false;
uint32_t g_lastStateSentAt = 0;
uint32_t g_lastProcessedAt = 0;
uint32_t g_pendingCrimeAt = 0;
uint32_t g_proximitySuspendedUntil = 0;
uint32_t g_localPlayerExitedVehicleAt = 0;
bool g_localPlayerWasInVehicle = false;
bool g_pendingCrime = false;
CVehicle* g_lastLocalSharedVehicle = nullptr;

struct RecentCrimeSource
{
    CVector position{};
    uint32_t recordedAt = 0;
    int playerId = -1;
};

constexpr size_t RECENT_CRIME_SOURCE_COUNT = 16;
constexpr uint32_t RECENT_CRIME_SOURCE_LIFETIME = 10000;
constexpr float RECENT_CRIME_SOURCE_RADIUS = 60.0f;
RecentCrimeSource g_recentCrimeSources[RECENT_CRIME_SOURCE_COUNT]{};
size_t g_nextRecentCrimeSource = 0;

struct RecentVehicleEntrySource
{
    unsigned int vehiclePointer = 0;
    uint32_t recordedAt = 0;
    int playerId = -1;
};

constexpr size_t RECENT_VEHICLE_ENTRY_SOURCE_COUNT = 8;
constexpr uint32_t RECENT_VEHICLE_ENTRY_SOURCE_LIFETIME = 10000;
RecentVehicleEntrySource g_recentVehicleEntrySources[RECENT_VEHICLE_ENTRY_SOURCE_COUNT]{};
size_t g_nextRecentVehicleEntrySource = 0;

struct RecentVehicleDriverSource
{
    CVector position{};
    uint32_t recordedAt = 0;
    int playerId = -1;
};

constexpr size_t RECENT_VEHICLE_DRIVER_SOURCE_COUNT = 4;
constexpr uint32_t RECENT_VEHICLE_DRIVER_SOURCE_LIFETIME = 5000;
RecentVehicleDriverSource g_recentVehicleDriverSources[RECENT_VEHICLE_DRIVER_SOURCE_COUNT]{};

struct ForcedWantedSuppression
{
    CWanted* wanted = nullptr;
    eCrimeType crimeType = static_cast<eCrimeType>(0);
};

bool g_remoteCrimeForcedWantedMayFollow = false;
ForcedWantedSuppression g_forcedWantedSuppression{};

void ClearForcedWantedSuppression()
{
    g_forcedWantedSuppression = {};
}

bool CrimeForcesPlayerWantedLevel(eCrimeType crimeType)
{
    return crimeType == CRIME_FIRE_WEAPON_HIT_PED || crimeType == CRIME_DAMAGE_COP_CAR ||
           static_cast<int>(crimeType) == 19;
}

void ArmForcedWantedSuppression(CWanted* wanted, eCrimeType crimeType)
{
    g_forcedWantedSuppression.wanted = wanted;
    g_forcedWantedSuppression.crimeType = crimeType;
}

uint8_t GetWantedLevel(CPlayerPed* player)
{
    if (!player || !player->GetWanted())
    {
        return 0;
    }
    return static_cast<uint8_t>(std::min(player->GetWantedLevel(), 6));
}

bool IsInVehicle(CPlayerPed* player)
{
    return player && player->m_nPedFlags.bInVehicle && player->m_pVehicle;
}

bool IsPoliceEngaged(CPlayerPed* player)
{
    if (!player || GetWantedLevel(player) == 0)
    {
        return false;
    }

    CWanted* wanted = player->GetWanted();
    bool starsFlashing = plugin::Command<Commands::WANTED_STARS_ARE_FLASHING>(0);
    return wanted && wanted->m_nCopsInPursuit > 0 && !starsFlashing;
}

bool IsSameSceneAndNearby(CPlayerPed* localPlayer, CPlayerPed* remotePlayer)
{
    if (!localPlayer || !remotePlayer || localPlayer->m_nAreaCode != remotePlayer->m_nAreaCode)
    {
        return false;
    }
    return (localPlayer->GetPosition() - remotePlayer->GetPosition()).Magnitude() <= PROXIMITY_RADIUS;
}

bool IsVehicleCollisionCrime(eCrimeType crimeType)
{
    return crimeType == CRIME_DAMAGED_PED || crimeType == CRIME_DAMAGE_CAR || crimeType == CRIME_DAMAGE_COP_CAR ||
           crimeType == CRIME_KILL_PED_WITH_CAR || crimeType == CRIME_KILL_COP_PED_WITH_CAR ||
           crimeType == CRIME_HIT_CAR;
}

CNetworkPlayer* ResolveRemoteCrimeOwner(
    eCrimeType crimeType, const CVector& position, unsigned int crimeId, CPlayerPed* localPlayer)
{
    uint32_t now = GetTickCount();

    if (crimeType == CRIME_CAR_STEAL)
    {
        RecentVehicleEntrySource* newestEntry = nullptr;
        uint32_t newestEntryAge = UINT32_MAX;
        for (auto& entry : g_recentVehicleEntrySources)
        {
            uint32_t age = now - entry.recordedAt;
            if (entry.recordedAt != 0 && entry.vehiclePointer == crimeId &&
                age <= RECENT_VEHICLE_ENTRY_SOURCE_LIFETIME && age < newestEntryAge)
            {
                newestEntry = &entry;
                newestEntryAge = age;
            }
        }

        if (newestEntry)
        {
            if (newestEntry->playerId == CNetworkPlayerManager::m_nMyId)
            {
                return nullptr;
            }
            if (auto* networkPlayer = CNetworkPlayerManager::GetPlayer(newestEntry->playerId))
            {
                return networkPlayer;
            }
        }
    }

    RecentCrimeSource* newestSource = nullptr;
    uint32_t newestSourceAge = UINT32_MAX;
    for (auto& source : g_recentCrimeSources)
    {
        if (source.recordedAt == 0)
        {
            continue;
        }

        uint32_t age = now - source.recordedAt;
        if (age <= RECENT_CRIME_SOURCE_LIFETIME && age < newestSourceAge &&
            (source.position - position).Magnitude() <= RECENT_CRIME_SOURCE_RADIUS)
        {
            newestSource = &source;
            newestSourceAge = age;
        }
    }

    if (newestSource)
    {
        if (newestSource->playerId == CNetworkPlayerManager::m_nMyId)
        {
            return nullptr;
        }
        if (auto* networkPlayer = CNetworkPlayerManager::GetPlayer(newestSource->playerId))
        {
            return networkPlayer;
        }
    }

    if (IsVehicleCollisionCrime(crimeType) && localPlayer &&
        (localPlayer->GetPosition() - position).Magnitude() > RECENT_CRIME_SOURCE_RADIUS)
    {
        RecentVehicleDriverSource* newestDriverSource = nullptr;
        uint32_t newestDriverSourceAge = UINT32_MAX;
        for (auto& source : g_recentVehicleDriverSources)
        {
            if (source.recordedAt == 0)
            {
                continue;
            }

            uint32_t age = now - source.recordedAt;
            float distance = (source.position - position).Magnitude();
            if (age <= RECENT_VEHICLE_DRIVER_SOURCE_LIFETIME && age < newestDriverSourceAge &&
                distance <= RECENT_CRIME_SOURCE_RADIUS)
            {
                newestDriverSource = &source;
                newestDriverSourceAge = age;
            }
        }

        if (newestDriverSource)
        {
            if (auto* networkPlayer = CNetworkPlayerManager::GetPlayer(newestDriverSource->playerId))
            {
                return networkPlayer;
            }
        }
    }

    // A burning vehicle reports its crime after the explosion/damage context has ended. If no explicit
    // source survived, assign a distant fire to the remote player standing at the crime instead of player 0.
    if (crimeType != CRIME_SET_CAR_ON_FIRE || !localPlayer ||
        (localPlayer->GetPosition() - position).Magnitude() <= RECENT_CRIME_SOURCE_RADIUS)
    {
        return nullptr;
    }

    CNetworkPlayer* closestRemotePlayer = nullptr;
    float closestDistance = RECENT_CRIME_SOURCE_RADIUS;
    for (auto* networkPlayer : CNetworkPlayerManager::m_pPlayers)
    {
        if (!networkPlayer || !networkPlayer->m_pPed || networkPlayer->m_pPed->m_nAreaCode != localPlayer->m_nAreaCode)
        {
            continue;
        }

        float distance = (networkPlayer->m_pPed->GetPosition() - position).Magnitude();
        if (distance <= closestDistance)
        {
            closestRemotePlayer = networkPlayer;
            closestDistance = distance;
        }
    }
    return closestRemotePlayer;
}

void SendLocalState(bool crimeCommitted)
{
    CPlayerPed* localPlayer = FindPlayerPed(0);
    if (!CNetwork::m_bAuthenticated || !localPlayer)
    {
        return;
    }

    Packets::Players::PlayerWantedLevel packet{};
    packet.wantedLevel = GetWantedLevel(localPlayer);
    packet.bPoliceEngaged = IsPoliceEngaged(localPlayer);
    packet.bCrimeCommitted = crimeCommitted;
    packet.reason = Packets::Players::eWantedSyncReason::PERSONAL;
    GetPacketFactory().Send(packet);

    g_lastObservedWantedLevel = packet.wantedLevel;
    g_lastObservedPoliceEngaged = packet.bPoliceEngaged;
    g_lastStateSentAt = GetTickCount();
}

void SetLocalWantedLevel(uint8_t wantedLevel, Packets::Players::eWantedSyncReason reason, bool suppressEcho)
{
    CPlayerPed* localPlayer = FindPlayerPed(0);
    if (!localPlayer || !localPlayer->GetWanted())
    {
        return;
    }

    wantedLevel = std::min<uint8_t>(wantedLevel, 6);
    uint8_t previousWantedLevel = GetWantedLevel(localPlayer);
    bool clearWantedState = wantedLevel == 0 && (reason == Packets::Players::eWantedSyncReason::VEHICLE_CLEAR ||
                                                    reason == Packets::Players::eWantedSyncReason::RESPAWN);
    if (clearWantedState)
    {
        localPlayer->GetWanted()->ClearQdCrimes();

        // Pay 'n' Spray puts the local driver on parole before its network clear is echoed back.
        // Re-entering parole at zero would overwrite the saved wanted level and stop the stars flashing.
        if (reason == Packets::Players::eWantedSyncReason::RESPAWN || previousWantedLevel > 0)
        {
            localPlayer->GetWanted()->ClearWantedLevelAndGoOnParole();
        }
    }
    else if (previousWantedLevel == wantedLevel)
    {
        if (suppressEcho)
        {
            g_lastObservedWantedLevel = wantedLevel;
        }
        return;
    }
    else
    {
        localPlayer->SetWantedLevel(wantedLevel);
    }

    if (suppressEcho)
    {
        g_lastObservedWantedLevel = GetWantedLevel(localPlayer);
    }
}

void ApplyRemoteWantedLevel(CNetworkPlayer* networkPlayer)
{
    if (!networkPlayer || !networkPlayer->m_pPed || !networkPlayer->m_pPed->GetWanted())
    {
        return;
    }

    uint8_t representedWantedLevel = GetWantedLevel(networkPlayer->m_pPed);
    if (representedWantedLevel != networkPlayer->m_nWantedLevel)
    {
        networkPlayer->m_pPed->SetWantedLevel(networkPlayer->m_nWantedLevel);
    }
}

bool ExitedRecently(uint32_t exitedAt, uint32_t now)
{
    return exitedAt != 0 && now - exitedAt <= EXIT_INVOLVEMENT_WINDOW;
}

void ProcessLocalSharing(CPlayerPed* localPlayer, uint32_t now)
{
    bool localPlayerInVehicle = IsInVehicle(localPlayer);
    if (g_localPlayerWasInVehicle && !localPlayerInVehicle)
    {
        g_localPlayerExitedVehicleAt = now;
    }
    g_localPlayerWasInVehicle = localPlayerInVehicle;

    uint8_t localWantedLevel = GetWantedLevel(localPlayer);

    CVehicle* currentVehicle = localPlayerInVehicle ? localPlayer->m_pVehicle : nullptr;
    bool enteredNewSharedVehicle = currentVehicle && currentVehicle != g_lastLocalSharedVehicle;
    g_lastLocalSharedVehicle = currentVehicle;

    if (enteredNewSharedVehicle)
    {
        uint8_t highestOccupantWantedLevel = localWantedLevel;
        for (auto* networkPlayer : CNetworkPlayerManager::m_pPlayers)
        {
            CPlayerPed* remotePlayer = networkPlayer ? networkPlayer->m_pPed : nullptr;
            if (IsInVehicle(remotePlayer) && remotePlayer->m_pVehicle == localPlayer->m_pVehicle)
            {
                highestOccupantWantedLevel = std::max(highestOccupantWantedLevel, networkPlayer->m_nWantedLevel);
            }
        }

        if (highestOccupantWantedLevel > localWantedLevel)
        {
            SetLocalWantedLevel(highestOccupantWantedLevel, Packets::Players::eWantedSyncReason::VEHICLE_SHARE, false);
            localWantedLevel = GetWantedLevel(localPlayer);
        }
    }

    for (auto* networkPlayer : CNetworkPlayerManager::m_pPlayers)
    {
        if (!networkPlayer || !networkPlayer->m_pPed)
        {
            continue;
        }

        CPlayerPed* remotePlayer = networkPlayer->m_pPed;
        bool remotePlayerInVehicle = IsInVehicle(remotePlayer);
        if (networkPlayer->m_bWantedWasInVehicle && !remotePlayerInVehicle)
        {
            networkPlayer->m_nWantedExitedVehicleAt = now;
        }
        networkPlayer->m_bWantedWasInVehicle = remotePlayerInVehicle;

        ApplyRemoteWantedLevel(networkPlayer);

        bool sameVehicle =
            localPlayerInVehicle && remotePlayerInVehicle && localPlayer->m_pVehicle == remotePlayer->m_pVehicle;
        bool sameArea = localPlayer->m_nAreaCode == remotePlayer->m_nAreaCode;
        float distance = (localPlayer->GetPosition() - remotePlayer->GetPosition()).Magnitude();
        bool nearby = sameArea && distance <= PROXIMITY_RADIUS;
        bool proximityActive = now >= g_proximitySuspendedUntil;
        bool canShareEncounter = proximityActive && !sameVehicle && networkPlayer->m_nWantedLevel > localWantedLevel &&
                                 networkPlayer->m_bPoliceEngaged && nearby;

        if (!canShareEncounter)
        {
            networkPlayer->m_nWantedEngagementStartedAt = 0;
            continue;
        }

        bool justEmergedIntoEncounter = ExitedRecently(g_localPlayerExitedVehicleAt, now) ||
                                        ExitedRecently(networkPlayer->m_nWantedExitedVehicleAt, now);
        if (justEmergedIntoEncounter)
        {
            SetLocalWantedLevel(networkPlayer->m_nWantedLevel, Packets::Players::eWantedSyncReason::PERSONAL, false);
            localWantedLevel = GetWantedLevel(localPlayer);
            networkPlayer->m_nWantedEngagementStartedAt = 0;
            continue;
        }

        if (networkPlayer->m_nWantedEngagementStartedAt == 0)
        {
            networkPlayer->m_nWantedEngagementStartedAt = now;
        }
        else if (now - networkPlayer->m_nWantedEngagementStartedAt >= POLICE_INVOLVEMENT_DELAY)
        {
            SetLocalWantedLevel(networkPlayer->m_nWantedLevel, Packets::Players::eWantedSyncReason::PERSONAL, false);
            localWantedLevel = GetWantedLevel(localPlayer);
            networkPlayer->m_nWantedEngagementStartedAt = 0;
        }
    }
}
void ResetLocalState()
{
    g_lastObservedWantedLevel = UNINITIALISED_WANTED_LEVEL;
    g_lastObservedPoliceEngaged = false;
    g_lastStateSentAt = 0;
    g_lastProcessedAt = 0;
    g_pendingCrimeAt = 0;
    g_proximitySuspendedUntil = 0;
    g_localPlayerExitedVehicleAt = 0;
    g_localPlayerWasInVehicle = false;
    g_pendingCrime = false;
    g_lastLocalSharedVehicle = nullptr;
    for (auto& source : g_recentCrimeSources)
    {
        source = {};
    }
    g_nextRecentCrimeSource = 0;
    for (auto& source : g_recentVehicleEntrySources)
    {
        source = {};
    }
    g_nextRecentVehicleEntrySource = 0;
    for (auto& source : g_recentVehicleDriverSources)
    {
        source = {};
    }
    g_remoteCrimeForcedWantedMayFollow = false;
    ClearForcedWantedSuppression();
}

void __fastcall CWanted__RegisterCrime_Hook(CWanted* wanted, SKIP_EDX, eCrimeType crimeType, const CVector& position,
    unsigned int crimeId, bool policeDontReallyCare)
{
    CPlayerPed* localPlayer = FindPlayerPed(0);
    bool localWanted = CWorld::PlayerInFocus == 0 && localPlayer && wanted == localPlayer->GetWanted();
    if (localWanted)
    {
        if (ResolveRemoteCrimeOwner(crimeType, position, crimeId, localPlayer))
        {
            return;
        }
        if (!policeDontReallyCare)
        {
            CWantedSync::NotifyLocalCrime();
        }
    }
    wanted->RegisterCrime(crimeType, position, crimeId, policeDontReallyCare);
}

void __fastcall CWanted__RegisterCrimeImmediately_Hook(CWanted* wanted, SKIP_EDX, eCrimeType crimeType,
    const CVector& position, unsigned int crimeId, bool policeDontReallyCare)
{
    CPlayerPed* localPlayer = FindPlayerPed(0);
    bool localWanted = CWorld::PlayerInFocus == 0 && localPlayer && wanted == localPlayer->GetWanted();
    if (localWanted)
    {
        if (ResolveRemoteCrimeOwner(crimeType, position, crimeId, localPlayer))
        {
            if (g_remoteCrimeForcedWantedMayFollow)
            {
                ArmForcedWantedSuppression(wanted, crimeType);
            }
            return;
        }
        if (!policeDontReallyCare)
        {
            CWantedSync::NotifyLocalCrime();
        }
    }
    wanted->RegisterCrime_Immediately(crimeType, position, crimeId, policeDontReallyCare);
}

void __fastcall CWanted__RegisterCrimeImmediatelyWithForcedWanted_Hook(CWanted* wanted, SKIP_EDX, eCrimeType crimeType,
    const CVector& position, unsigned int crimeId, bool policeDontReallyCare)
{
    g_remoteCrimeForcedWantedMayFollow = true;
    CWanted__RegisterCrimeImmediately_Hook(wanted, nullptr, crimeType, position, crimeId, policeDontReallyCare);
    g_remoteCrimeForcedWantedMayFollow = false;
}

void __fastcall CWanted__SetWantedLevelNoDropAfterCrime_Hook(CWanted* wanted, SKIP_EDX, int level)
{
    if (g_forcedWantedSuppression.wanted == wanted)
    {
        if (!CrimeForcesPlayerWantedLevel(g_forcedWantedSuppression.crimeType))
        {
            ClearForcedWantedSuppression();
        }
        return;
    }

    ClearForcedWantedSuppression();
    wanted->SetWantedLevelNoDrop(level);
}

void __fastcall CPlayerPed__SetWantedLevelNoDropAfterCrime_Hook(CPlayerPed* player, SKIP_EDX, int level)
{
    if (player && g_forcedWantedSuppression.wanted == player->GetWanted())
    {
        ClearForcedWantedSuppression();
        return;
    }

    ClearForcedWantedSuppression();
    player->SetWantedLevelNoDrop(level);
}

bool __cdecl CPickups__GivePlayerGoodiesWithPickUpMI_Hook(unsigned short modelId, int playerId)
{
    CPlayerPed* localPlayer = playerId == 0 ? FindPlayerPed(0) : nullptr;
    CWanted* wanted = localPlayer ? localPlayer->GetWanted() : nullptr;
    bool networkVehicleBribe = modelId == MODEL_BRIBE && playerId == 0 && IsInVehicle(localPlayer) && wanted;

    unsigned int wantedLevelBefore = wanted ? wanted->m_nWantedLevel : 0;
    unsigned int chaosBefore = wanted ? wanted->m_nChaosLevel : 0;
    unsigned int chaosBeforeParole = wanted ? wanted->m_nChaosLevelBeforeParole : 0;
    unsigned int lastDecreaseBefore = wanted ? wanted->m_nLastTimeWantedDecreased : 0;
    unsigned int lastLevelChangeBefore = wanted ? wanted->m_nLastTimeWantedLevelChanged : 0;
    unsigned int wantedBeforeParole = wanted ? wanted->m_nWantedLevelBeforeParole : 0;

    bool pickedUp = CPickups::GivePlayerGoodiesWithPickUpMI(modelId, playerId);
    if (pickedUp && networkVehicleBribe)
    {
        wanted->m_nWantedLevel = wantedLevelBefore;
        wanted->m_nChaosLevel = chaosBefore;
        wanted->m_nChaosLevelBeforeParole = chaosBeforeParole;
        wanted->m_nLastTimeWantedDecreased = lastDecreaseBefore;
        wanted->m_nLastTimeWantedLevelChanged = lastLevelChangeBefore;
        wanted->m_nWantedLevelBeforeParole = wantedBeforeParole;
        CWantedSync::NotifyPoliceBribe();
    }
    return pickedUp;
}

void __fastcall CWanted__ClearWantedLevelAndGoOnParole_GarageHook(CWanted* wanted, SKIP_EDX)
{
    wanted->ClearWantedLevelAndGoOnParole();
    CWantedSync::NotifyPayNSpray();
}
}  // namespace

void CWantedSync::Process()
{
    if (!CNetwork::m_bAuthenticated)
    {
        ResetLocalState();
        return;
    }

    CPlayerPed* localPlayer = FindPlayerPed(0);
    if (!localPlayer || !localPlayer->GetWanted())
    {
        return;
    }

    uint32_t now = GetTickCount();
    if (now - g_lastProcessedAt < PROCESS_RATE)
    {
        return;
    }
    g_lastProcessedAt = now;

    if (g_lastObservedWantedLevel == UNINITIALISED_WANTED_LEVEL)
    {
        g_lastObservedWantedLevel = GetWantedLevel(localPlayer);
        g_lastObservedPoliceEngaged = IsPoliceEngaged(localPlayer);
        SendLocalState(false);
    }

    ProcessLocalSharing(localPlayer, now);

    uint8_t wantedLevel = GetWantedLevel(localPlayer);
    bool policeEngaged = IsPoliceEngaged(localPlayer);
    bool stateChanged = wantedLevel != g_lastObservedWantedLevel || policeEngaged != g_lastObservedPoliceEngaged;
    bool crimeReady = g_pendingCrime && wantedLevel > 0;

    if (stateChanged || crimeReady || now - g_lastStateSentAt >= STATE_HEARTBEAT_RATE)
    {
        SendLocalState(crimeReady);
        if (crimeReady)
        {
            g_pendingCrime = false;
        }
    }

    if (g_pendingCrime && now - g_pendingCrimeAt > 500 && wantedLevel == 0)
    {
        g_pendingCrime = false;
    }
}

void CWantedSync::HandlePacket(Packets::Players::PlayerWantedLevel* packet)
{
    if (!packet)
    {
        return;
    }

    packet->wantedLevel = std::min<uint8_t>(packet->wantedLevel, 6);
    uint32_t now = GetTickCount();

    if (packet->playerid.value == CNetworkPlayerManager::m_nMyId)
    {
        if (packet->reason == Packets::Players::eWantedSyncReason::VEHICLE_CLEAR)
        {
            g_proximitySuspendedUntil = now + RESPRAY_PROXIMITY_SUSPEND;
        }
        else if (packet->reason == Packets::Players::eWantedSyncReason::RESPAWN)
        {
            g_localPlayerExitedVehicleAt = 0;
            g_localPlayerWasInVehicle = false;
        }
        SetLocalWantedLevel(packet->wantedLevel, packet->reason, true);
        return;
    }

    CNetworkPlayer* networkPlayer = CNetworkPlayerManager::GetPlayer(packet->playerid);
    if (!networkPlayer)
    {
        return;
    }

    CPlayerPed* localPlayer = FindPlayerPed(0);
    bool sameVehicle = localPlayer && networkPlayer->m_pPed && IsInVehicle(localPlayer) &&
                       IsInVehicle(networkPlayer->m_pPed) &&
                       localPlayer->m_pVehicle == networkPlayer->m_pPed->m_pVehicle;

    networkPlayer->m_nWantedLevel = packet->wantedLevel;
    networkPlayer->m_bPoliceEngaged = packet->wantedLevel > 0 && packet->bPoliceEngaged;
    ApplyRemoteWantedLevel(networkPlayer);

    if (packet->reason == Packets::Players::eWantedSyncReason::VEHICLE_CLEAR && sameVehicle)
    {
        g_proximitySuspendedUntil = now + RESPRAY_PROXIMITY_SUSPEND;
    }
    else if (packet->reason == Packets::Players::eWantedSyncReason::RESPAWN)
    {
        networkPlayer->m_bWantedWasInVehicle = false;
        networkPlayer->m_nWantedExitedVehicleAt = 0;
        networkPlayer->m_nWantedEngagementStartedAt = 0;
    }

    if (packet->bCrimeCommitted)
    {
        bool crimeTransferAllowed = now >= g_proximitySuspendedUntil && localPlayer &&
                                    networkPlayer->m_nWantedLevel > GetWantedLevel(localPlayer) &&
                                    IsSameSceneAndNearby(localPlayer, networkPlayer->m_pPed);
        if (crimeTransferAllowed)
        {
            SetLocalWantedLevel(networkPlayer->m_nWantedLevel, Packets::Players::eWantedSyncReason::PERSONAL, false);
        }
    }
}

void CWantedSync::RecordCrimeSource(CPed* creator, const CVector& position)
{
    if (!creator || !CNetwork::m_bAuthenticated)
    {
        return;
    }

    int playerId = -1;
    if (creator == FindPlayerPed(0))
    {
        playerId = CNetworkPlayerManager::m_nMyId;
    }
    else if (auto* networkPlayer = CNetworkPlayerManager::GetPlayer(creator))
    {
        playerId = networkPlayer->m_iPlayerId;
    }

    if (playerId < 0)
    {
        return;
    }

    RecentCrimeSource& source = g_recentCrimeSources[g_nextRecentCrimeSource];
    source.position = position;
    source.recordedAt = GetTickCount();
    source.playerId = playerId;
    g_nextRecentCrimeSource = (g_nextRecentCrimeSource + 1) % RECENT_CRIME_SOURCE_COUNT;
}

void CWantedSync::RecordVehicleEntrySource(CPed* player, CVehicle* vehicle)
{
    if (!player || !vehicle || !CNetwork::m_bAuthenticated)
    {
        return;
    }

    int playerId = -1;
    if (player == FindPlayerPed(0))
    {
        playerId = CNetworkPlayerManager::m_nMyId;
    }
    else if (auto* networkPlayer = CNetworkPlayerManager::GetPlayer(player))
    {
        playerId = networkPlayer->m_iPlayerId;
    }

    if (playerId < 0)
    {
        return;
    }

    RecentVehicleEntrySource& source = g_recentVehicleEntrySources[g_nextRecentVehicleEntrySource];
    source.vehiclePointer = static_cast<unsigned int>(reinterpret_cast<uintptr_t>(vehicle));
    source.recordedAt = GetTickCount();
    source.playerId = playerId;
    g_nextRecentVehicleEntrySource = (g_nextRecentVehicleEntrySource + 1) % RECENT_VEHICLE_ENTRY_SOURCE_COUNT;
}

void CWantedSync::RecordVehicleDriverSource(CPed* driver, const CVector& position)
{
    if (!driver || !CNetwork::m_bAuthenticated)
    {
        return;
    }

    CNetworkPlayer* networkPlayer = CNetworkPlayerManager::GetPlayer(driver);
    if (!networkPlayer)
    {
        return;
    }

    RecentVehicleDriverSource* target = nullptr;
    RecentVehicleDriverSource* oldest = &g_recentVehicleDriverSources[0];
    for (auto& source : g_recentVehicleDriverSources)
    {
        if (source.playerId == networkPlayer->m_iPlayerId)
        {
            target = &source;
            break;
        }
        if (source.recordedAt == 0)
        {
            target = &source;
        }
        if (source.recordedAt < oldest->recordedAt)
        {
            oldest = &source;
        }
    }

    if (!target)
    {
        target = oldest;
    }
    target->position = position;
    target->recordedAt = GetTickCount();
    target->playerId = networkPlayer->m_iPlayerId;
}

void CWantedSync::NotifyLocalCrime()
{
    if (!CNetwork::m_bAuthenticated)
    {
        return;
    }
    g_pendingCrime = true;
    g_pendingCrimeAt = GetTickCount();
}

void CWantedSync::NotifyPayNSpray()
{
    CPlayerPed* localPlayer = FindPlayerPed(0);
    if (!CNetwork::m_bAuthenticated || !IsInVehicle(localPlayer) || localPlayer->m_pVehicle->m_pDriver != localPlayer)
    {
        return;
    }

    uint32_t now = GetTickCount();
    g_proximitySuspendedUntil = now + RESPRAY_PROXIMITY_SUSPEND;
    Packets::Players::VehicleWantedAction packet{};
    packet.action = Packets::Players::eVehicleWantedAction::CLEAR;
    GetPacketFactory().Send(packet);
}

void CWantedSync::NotifyPoliceBribe()
{
    CPlayerPed* localPlayer = FindPlayerPed(0);
    if (!CNetwork::m_bAuthenticated || !IsInVehicle(localPlayer))
    {
        return;
    }

    Packets::Players::VehicleWantedAction packet{};
    packet.action = Packets::Players::eVehicleWantedAction::BRIBE;
    GetPacketFactory().Send(packet);
}

bool CWantedSync::AnyRemotePlayerWantedAbove(uint8_t wantedLevel)
{
    for (auto* networkPlayer : CNetworkPlayerManager::m_pPlayers)
    {
        if (networkPlayer && networkPlayer->m_nWantedLevel > wantedLevel)
        {
            return true;
        }
    }
    return false;
}

void CWantedSync::InjectHooks()
{
    patch::RedirectCall(0x4B2838, CWanted__RegisterCrime_Hook);
    patch::RedirectCall(0x4B3593, CWanted__RegisterCrime_Hook);
    patch::RedirectCall(0x4B4ECB, CWanted__RegisterCrime_Hook);
    patch::RedirectCall(0x4B510D, CWanted__RegisterCrime_Hook);

    patch::RedirectCall(0x5321BD, CWanted__RegisterCrime_Hook);

    patch::RedirectCall(0x5321E2, CWanted__RegisterCrimeImmediatelyWithForcedWanted_Hook);
    patch::RedirectCall(0x5321FB, CWanted__SetWantedLevelNoDropAfterCrime_Hook);
    patch::RedirectCall(0x532221, CPlayerPed__SetWantedLevelNoDropAfterCrime_Hook);
    patch::RedirectCall(0x655FA7, CWanted__RegisterCrimeImmediately_Hook);
    patch::RedirectCall(0x655FCD, CWanted__RegisterCrimeImmediately_Hook);

    patch::RedirectCall(0x44AF10, CWanted__ClearWantedLevelAndGoOnParole_GarageHook);
    patch::RedirectCall(0x457DF2, CPickups__GivePlayerGoodiesWithPickUpMI_Hook);
    patch::RedirectCall(0x457F91, CPickups__GivePlayerGoodiesWithPickUpMI_Hook);
}
