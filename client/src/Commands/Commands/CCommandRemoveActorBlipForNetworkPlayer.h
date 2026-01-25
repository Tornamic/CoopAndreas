#pragma once
#include "../CCustomCommand.h"
class CCommandRemoveActorBlipForNetworkPlayer :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

