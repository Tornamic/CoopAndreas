#pragma once
#include "../CCustomCommand.h"
class CCommandUpdateCheckpointForNetworkPlayer :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

