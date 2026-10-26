#pragma once
#include "../CCustomCommand.h"
class CCommandCanScriptControlTrain :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};
