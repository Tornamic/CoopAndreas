#include "../stdafx.h"
#include "PedHooks.h"

static void __cdecl CPopulation__Update_Hook(bool generate)
{
    if (CLocalPlayer::m_bIsHost)
        CPopulation::Update(generate);
}

void PedHooks::InjectHooks()
{
    // ped hooks
    patch::RedirectCall(0x53C030, CPopulation__Update_Hook);
    patch::RedirectCall(0x53C054, CPopulation__Update_Hook);
}
