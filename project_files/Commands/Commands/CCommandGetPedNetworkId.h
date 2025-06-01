#pragma once
#include "../CCustomCommand.h"
class CCommandGetPedNetworkId :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

