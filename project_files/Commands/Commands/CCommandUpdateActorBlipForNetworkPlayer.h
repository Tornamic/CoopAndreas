#pragma once
#include "../CCustomCommand.h"
class CCommandUpdateActorBlipForNetworkPlayer :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

