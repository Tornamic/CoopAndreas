#pragma once
#include "../CCustomCommand.h"
class CCommandRemoveTextForNetworkPlayer :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

