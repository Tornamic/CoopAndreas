#include "ShadowHooks.h"

// fixes realtime shadows of players

struct CRealTimeShadowManager
{
    bool m_bInitialized;
    bool m_bNotReInitialized;
    CRealTimeShadow* m_ShadowData[16];
    CShadowCamera m_BlurCamera;
    CShadowCamera m_GradientCamera;
};
VALIDATE_SIZE(CRealTimeShadowManager, 0x54);

void __fastcall CRealTimeShadowManager__ReturnRealTimeShadow_Reimpl(
    CRealTimeShadowManager* This, SKIP_EDX, CRealTimeShadow* shdData)
{
    if (This->m_bInitialized && shdData->m_pOwner)
    {
        shdData->m_pOwner->m_pShadowData = nullptr;
        shdData->m_pOwner = nullptr;
    }
}

void __declspec(naked) CRealTimeShadowManager__GetRealTimeShadow_Hook()
{
    static CRealTimeShadowManager* pShadowMgr;
    static CRealTimeShadow* pShadow;
    static int i;

    __asm
    {
        mov pShadowMgr, ecx
    }

    pShadow = nullptr;

    for (i = 0; i < ARRAY_SIZE(pShadowMgr->m_ShadowData); i++)
    {
        if (pShadowMgr->m_ShadowData[i] && pShadowMgr->m_ShadowData[i]->m_pOwner == nullptr)
        {
            pShadow = pShadowMgr->m_ShadowData[i];
        }
    }

    __asm
    {
        mov esi, pShadow
        
        push 0x706A3B
        retn
    }
}

void ShadowHooks::InjectHooks()
{
    // check `shdData->m_pOwner` for nullptr
    patch::ReplaceFunction(0x705B30, CRealTimeShadowManager__ReturnRealTimeShadow_Reimpl);

    // dont force m_ShadowData[0] 0x7069F7 for all players
    patch::SetUChar(0x7069F5, 0xEB);

    // instead, make the shadow mgr use all 16 slots taking into account the hardcoded m_ShadowData[0] 0x7069F7
    // in MTA:SA they had a simpler solution?
    patch::RedirectJump(0x7069FC, CRealTimeShadowManager__GetRealTimeShadow_Hook);
}
