#pragma once
#include "../CCustomCommand.h"
class CCommandCancelPedClaim :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

