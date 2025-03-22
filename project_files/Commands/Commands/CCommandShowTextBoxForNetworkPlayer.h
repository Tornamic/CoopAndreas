#pragma once
#include "../CCustomCommand.h"
class CCommandShowTextBoxForNetworkPlayer :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

