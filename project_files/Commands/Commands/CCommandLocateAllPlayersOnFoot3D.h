#pragma once
#include "../CCustomCommand.h"
class CCommandLocateAllPlayersOnFoot3D :
    public CCustomCommand
{
    // Inherited via CCustomCommand
    void Process(CRunningScript* script) override;
};

