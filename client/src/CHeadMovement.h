#pragma once

#include "stdafx.h"

class CHeadMovement
{
public:
    static void Process();
    static void HandlePacket(Packets::Players::HeadMovement* pHeadMovement, CNetworkPlayer* pNetworkPlayer);
};