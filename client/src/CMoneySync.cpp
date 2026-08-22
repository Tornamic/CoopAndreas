#include "stdafx.h"
#include "CMoneySync.h"
#include "network/packets/world.h"

static int32_t s_lastSentMoney = -1;

void CMoneySync::SyncCurrentState()
{
    if (!CNetwork::m_bAuthenticated)
        return;

    const int32_t money = CWorld::Players[0].m_nMoney;

    if (money == s_lastSentMoney)
        return;

    s_lastSentMoney = money;

    Packets::World::GameMoneySync packet{};
    packet.money = money;
    GetPacketFactory().Send(packet);
}

void CMoneySync::HandlePacket(Packets::World::GameMoneySync* packet)
{
    if (!packet)
        return;

    CWorld::Players[0].m_nMoney = packet->money;
    CWorld::Players[0].m_nDisplayMoney = packet->money;
    s_lastSentMoney = packet->money;
}