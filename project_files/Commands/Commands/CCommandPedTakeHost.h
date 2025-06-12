#pragma once
#include "../CCustomCommand.h"
class CCommandPedTakeHost :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

