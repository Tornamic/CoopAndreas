#pragma once

#include "stdafx.h"

// money is shared by all players: we send our own changes, the server sends back the total
class CMoneySync
{
public:
    static void Reset();
    static void Process();
    static void HandlePacket(Packets::World::SetMoney* pSetMoney);

private:
    static inline int32_t ms_nLastSyncedMoney = 0;
};
