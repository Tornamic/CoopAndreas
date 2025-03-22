#pragma once
#include "../CCustomCommand.h"
class CCommandShowTextLowPriorityForNetworkPlayer :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

