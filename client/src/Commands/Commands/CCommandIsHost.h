#pragma once
#include "../CCustomCommand.h"
class CCommandIsHost :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

