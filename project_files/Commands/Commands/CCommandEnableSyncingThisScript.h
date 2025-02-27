#pragma once
#include "../CCustomCommand.h"
class CCommandEnableSyncingThisScript :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

