#pragma once
#include "../CCustomCommand.h"
class CCommandShowTextHighPriorityForNetworkPlayer :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

