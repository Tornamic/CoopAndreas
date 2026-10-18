#include "stdafx.h"
#include "CHorseshoeSync.h"
#include "CPickups.h"

namespace
{
    constexpr bool kDebug = false;

    struct SPos { float x, y, z; };

    // Same order as the $pickup_horseshoe array in scm/scripts/INITIAL.txt (globals 2882..2931)
    constexpr SPos kPos[CHorseshoeSync::COUNT] = {
        {1224.0f, 2617.0f, 11.0f},  {2323.0f, 1284.0f, 97.0f},  {2035.0f, 2305.0f, 18.0f},
        {2491.0f, 2263.0f, 15.0f},  {1433.0f, 2796.0f, 20.0f},  {2071.0f, 712.0f, 11.0f},
        {2239.0f, 1839.0f, 18.0f},  {2583.0f, 2387.0f, 16.0f},  {2864.0f, 857.0f, 13.0f},
        {2612.0f, 2200.0f, -1.0f},  {2274.0f, 1507.0f, 24.0f},  {2184.0f, 2529.0f, 11.0f},
        {1863.0f, 2314.0f, 15.0f},  {2054.0f, 2434.0f, 166.0f}, {1603.0f, 1435.0f, 11.0f},
        {1362.92f, 1015.24f, 11.0f},{2058.7f, 2159.1f, 16.0f},  {2003.0f, 1672.0f, 12.0f},
        {2238.0f, 1135.0f, 49.0f},  {1934.06f, 988.79f, 22.0f}, {1768.0f, 2847.0f, 9.0f},
        {1084.0f, 1076.0f, 11.0f},  {2879.0f, 2522.0f, 11.0f},  {2371.0f, 2009.0f, 15.0f},
        {1521.0f, 1690.0f, 10.6f},  {2417.0f, 1281.0f, 21.0f},  {1376.0f, 2304.0f, 15.0f},
        {1393.0f, 1832.0f, 12.34f}, {984.0f, 2563.0f, 12.0f},   {1767.0f, 601.0f, 13.0f},
        {2108.0f, 1003.0f, 46.0f},  {2705.98f, 1862.52f, 24.41f},{2493.0f, 922.0f, 16.0f},
        {1881.0f, 2846.0f, 11.0f},  {2020.0f, 2352.0f, 11.0f},  {1680.3f, 2226.86f, 16.11f},
        {1462.0f, 936.0f, 10.0f},   {2125.5f, 789.23f, 11.45f}, {2588.0f, 1902.0f, 15.0f},
        {919.0f, 2070.0f, 11.0f},   {2173.0f, 2465.0f, 11.0f},  {2031.25f, 2207.33f, 11.0f},
        {2509.0f, 1144.0f, 19.0f},  {2215.0f, 1968.0f, 11.0f},  {2626.0f, 2841.0f, 11.0f},
        {2440.08f, 2161.07f, 20.0f},{1582.0f, 2401.0f, 19.0f},  {2077.0f, 1912.0f, 14.0f},
        {970.0f, 1787.0f, 11.0f},   {1526.22f, 751.0f, 29.04f},
    };

    constexpr uint64_t Bit(int i) { return 1ull << i; }

    // CTheScripts::ScriptSpace (GTA SA 1.0 US), global variable N lives at +N*4
    int GetPickupHandle(int index)
    {
        return *reinterpret_cast<int*>(0xA49960 + 4 * (CHorseshoeSync::FIRST_GLOBAL_VAR + index));
    }

    void SendMask(uint64_t mask)
    {
        Packets::System::HorseshoeState packet{};
        packet.maskLow = static_cast<uint32_t>(mask & 0xFFFFFFFFull);
        packet.maskHigh = static_cast<uint32_t>(mask >> 32);
        GetPacketFactory().Send(packet);
    }
}  // namespace

void CHorseshoeSync::Process()
{
    if (!CNetwork::m_bConnected || !CNetwork::m_bAuthenticated)
    {
        ms_lastStat = -1.0f;
        return;
    }

    // 1) did the local player just collect a horseshoe?
    const float stat = CStats::GetStatValue(241);
    if (ms_lastStat >= 0.0f && stat > ms_lastStat)
    {
        if (CPed* player = FindPlayerPed(-1))
        {
            const CVector& p = player->GetPosition();
            int best = -1;
            float bestDist = 15.0f;
            for (int i = 0; i < COUNT; i++)
            {
                if (ms_collected & Bit(i))
                    continue;
                const float dx = p.x - kPos[i].x, dy = p.y - kPos[i].y, dz = p.z - kPos[i].z;
                const float dist = sqrtf(dx * dx + dy * dy + dz * dz);
                if (dist < bestDist)
                {
                    bestDist = dist;
                    best = i;
                }
            }
            if (best != -1)
            {
                ms_collected |= Bit(best);
                SendMask(Bit(best));
                if (kDebug)
                    CChat::AddMessage("{ffff00}[HS] sent horseshoe #%d", best);
            }
            else if (kDebug)
                CChat::AddMessage("{ffff00}[HS] stat rose but no horseshoe within 15m");
        }
    }

    // 2) apply what other players collected
    for (int i = 0; i < COUNT && ms_pendingRemote; i++)
    {
        const uint64_t bit = Bit(i);
        if (!(ms_pendingRemote & bit))
            continue;
        if (ms_collected & bit)
        {
            ms_pendingRemote &= ~bit;
            continue;
        }
        const int handle = GetPickupHandle(i);
        if (handle == 0 || handle == -1)
            continue;  // script hasn't created it yet, retry next frame

        ms_pendingRemote &= ~bit;
        ms_collected |= bit;
        CPickups::RemovePickUp(handle);
        CPickups::PickedUpHorseShoe();
        if (kDebug)
            CChat::AddMessage("{ffff00}[HS] applied remote horseshoe #%d", i);
    }

    // 3) new baseline (includes the increments we just caused ourselves)
    ms_lastStat = CStats::GetStatValue(241);
}

void CHorseshoeSync::SyncCurrentState()
{
    if (ms_collected != 0)
        SendMask(ms_collected);
}

void CHorseshoeSync::OnStateReceived(uint32_t low, uint32_t high)
{
    ms_pendingRemote |= (static_cast<uint64_t>(high) << 32) | low;
}