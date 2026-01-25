#include "stdafx.h"
#include "CCommandGetPedInAreaWithModel.h"

void CCommandGetPedInAreaWithModel::Process(CRunningScript* script)
{
	script->CollectParameters(9);
	CVector position = *(CVector*)&ScriptParams[0];
	CVector radius   = *(CVector*)&ScriptParams[3];
	int* models = (int*)&ScriptParams[6]; // array size -- 3
    CPed* foundPed = nullptr;

    ScriptParams[0] = -1;

	for (auto ped : CPools::ms_pPedPool)
	{
        if (!ped || ped->IsPlayer())
            continue;

        auto& pedPos = ped->GetPosition();

        if (
            pedPos.x >= position.x - radius.x && pedPos.x <= position.x + radius.x &&
            pedPos.y >= position.y - radius.y && pedPos.y <= position.y + radius.y &&
            pedPos.z >= position.z - radius.z && pedPos.z <= position.z + radius.z
        )
        {
            if (models[0] == -1 && models[1] == -1 && models[2] == -1)
            {
                ScriptParams[0] = CPools::GetPedRef(ped);
                break;
            }
            else
            {
                int modelId = ped->m_nModelIndex;
                if (modelId == models[0] || modelId == models[1] || modelId == models[2])
                {
                    ScriptParams[0] = CPools::GetPedRef(ped);
                    break;
                }
            }
        }
	}

    script->StoreParameters(1);
}
