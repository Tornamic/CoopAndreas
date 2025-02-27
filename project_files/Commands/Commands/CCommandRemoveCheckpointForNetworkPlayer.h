#pragma once
#include "../CCustomCommand.h"
class CCommandRemoveCheckpointForNetworkPlayer :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

