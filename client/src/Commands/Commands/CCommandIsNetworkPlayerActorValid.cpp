#include "stdafx.h"
#include "CCommandIsNetworkPlayerActorValid.h"

void CCommandIsNetworkPlayerActorValid::Process(CRunningScript* script)
{
    script->CollectParameters(1);

    if (!ScriptParams[0])
    {
        script->UpdateCompareFlag(false);
        return;
    }

    if (auto ped = CPools::GetPed(ScriptParams[0]))
    {
        script->UpdateCompareFlag(CNetworkPlayerManager::GetPlayer(ped) != nullptr);
    }
    else
    {
        script->UpdateCompareFlag(false);
    }
}

