#pragma once
#include "../CCustomCommand.h"
class CCommandTeleportPlayersToHostSafely :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

