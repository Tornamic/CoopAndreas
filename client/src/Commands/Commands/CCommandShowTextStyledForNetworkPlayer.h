#pragma once
#include "../CCustomCommand.h"
class CCommandShowTextStyledForNetworkPlayer :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

