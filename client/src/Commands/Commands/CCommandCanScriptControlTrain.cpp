#include "stdafx.h"
#include "CCommandCanScriptControlTrain.h"

void CCommandCanScriptControlTrain::Process(CRunningScript* script)
{
    script->CollectParameters(1);

    bool bResult = true;
    if (CVehicle* pVehicle = CPools::GetVehicle(ScriptParams[0]))
    {
        if (pVehicle->m_nVehicleType == VEHICLE_TRAIN)
        {
            if (CVehicle* pEngine = CTrain::FindEngine((CTrain*)pVehicle))
            {
                if (CNetworkVehicle* pNetworkEngine = CNetworkVehicleManager::GetVehicle(pEngine))
                {
                    if (CNetworkPlayerManager::GetPlayer(pEngine->m_pDriver) != nullptr || !pNetworkEngine->m_bSyncing)
                    {
                        bResult = false;
                    }
                }
            }
        }
    }
    script->UpdateCompareFlag(bResult);
}
