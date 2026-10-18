#include "stdafx.h"
#include "CMoneySync.h"

void CMoneySync::Reset()
{
    // whatever we had before connecting is not added to the shared money
    ms_nLastSyncedMoney = CWorld::Players[0].m_nMoney;
}

void CMoneySync::Process()
{
    int32_t nMoney = CWorld::Players[0].m_nMoney;
    if (nMoney == ms_nLastSyncedMoney)
    {
        return;
    }

    Packets::World::AddMoney packet{};
    packet.delta = nMoney - ms_nLastSyncedMoney;
    GetPacketFactory().Send(packet);

    ms_nLastSyncedMoney = nMoney;
}

void CMoneySync::HandlePacket(Packets::World::SetMoney* pSetMoney)
{
    // send what changed since the last frame first, otherwise it would be overwritten by the total
    Process();

    CWorld::Players[0].m_nMoney = pSetMoney->money;
    ms_nLastSyncedMoney = pSetMoney->money;
}
