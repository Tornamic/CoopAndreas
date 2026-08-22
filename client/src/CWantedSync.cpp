#include "stdafx.h"
#include "CWantedSync.h"
#include "network/packets/world.h"

static int s_lastSentWanted = -1;

static CWanted* GetLocalWanted()
{ return FindPlayerWanted(-1); }

void CWantedSync::SyncCurrentState()
{
    if (!CNetwork::m_bAuthenticated)
        return;

    CWanted* wanted = GetLocalWanted();
    if (!wanted)
        return;

    const int level = static_cast<int>(wanted->m_nWantedLevel);
    if (level == s_lastSentWanted)
        return;

    s_lastSentWanted = level;

    Packets::World::GameWantedSync packet{};
    packet.wantedLevel = static_cast<uint8_t>(level);
    GetPacketFactory().Send(packet);
}

void CWantedSync::HandlePacket(Packets::World::GameWantedSync* packet)
{
    if (!packet)
        return;

    CWanted* wanted = GetLocalWanted();
    if (!wanted)
        return;

    const int level = static_cast<int>(packet->wantedLevel);
    if (level < 0 || level > 6)
        return;

    wanted->SetWantedLevel(level);
    s_lastSentWanted = level;
}