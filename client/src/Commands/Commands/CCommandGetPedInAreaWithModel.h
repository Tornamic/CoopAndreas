#pragma once
#include "../CCustomCommand.h"
class CCommandGetPedInAreaWithModel :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

