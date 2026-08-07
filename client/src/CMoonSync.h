#pragma once

#include "stdafx.h"

class CMoonSync
{
public:
    static void SyncCurrentState();
    static void HandlePacket(Packets::World::UpdateMoonSize* pUpdateMoonSize);
};