#pragma once
#include "../CCustomCommand.h"
class CCommandClearAllEntityBlipsForNetworkPlayer :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

