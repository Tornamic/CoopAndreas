#pragma once
#include "../CCustomCommand.h"
class CCommandIsPointOnAnyPlayersScreen :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

