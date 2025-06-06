#pragma once
#include "../CCustomCommand.h"
class CCommandIsSyncingThisPed :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

