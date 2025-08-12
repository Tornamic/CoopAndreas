#pragma once
#include "../CCustomCommand.h"
class CCommandGetNetworkPlayerInternalId :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

