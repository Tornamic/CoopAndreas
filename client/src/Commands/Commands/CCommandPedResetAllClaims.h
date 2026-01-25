#pragma once
#include "../CCustomCommand.h"
class CCommandPedResetAllClaims :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

