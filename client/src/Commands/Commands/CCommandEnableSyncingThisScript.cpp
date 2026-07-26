#include "stdafx.h"
#include "CCommandEnableSyncingThisScript.h"
#include "COpCodeSync.h"

void CCommandEnableSyncingThisScript::Process(CRunningScript* pScript)
{
    assert(COpCodeSync::ms_iFreeSyncedScript < ARRAY_SIZE(COpCodeSync::ms_aszSyncedScripts));

    // CChat::AddMessage("CCommandEnableSyncingThisScript \"%s\"", pScript->m_szName);

    for (size_t i = 0; i < COpCodeSync::ms_iFreeSyncedScript; i++)
    {
        if (strnicmp(COpCodeSync::ms_aszSyncedScripts[i], pScript->m_szName, 7) == 0) 
        {
            return;
        }
    }

    strncpy_s(COpCodeSync::ms_aszSyncedScripts[COpCodeSync::ms_iFreeSyncedScript], 8, pScript->m_szName, 7);
    ++COpCodeSync::ms_iFreeSyncedScript;
}
