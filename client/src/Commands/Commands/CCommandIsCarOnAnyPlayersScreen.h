#pragma once
#include "../CCustomCommand.h"
class CCommandIsCarOnAnyPlayersScreen :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

