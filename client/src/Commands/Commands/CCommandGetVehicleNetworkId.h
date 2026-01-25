#pragma once
#include "../CCustomCommand.h"
class CCommandGetVehicleNetworkId :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

