#include "stdafx.h"
#include "CCommandEnableSyncingThisScript.h"
#include "COpCodeSync.h"

void CCommandEnableSyncingThisScript::Process(CRunningScript* script)
{
	CChat::AddMessage("CCommandEnableSyncingThisScript \"%s\"", script->m_szName);

    if (std::find(COpCodeSync::ms_vSyncedScripts.begin(), COpCodeSync::ms_vSyncedScripts.end(), script) == COpCodeSync::ms_vSyncedScripts.end())
    {
        COpCodeSync::ms_vSyncedScripts.push_back(script);
    }
}
