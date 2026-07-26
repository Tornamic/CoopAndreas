#include "stdafx.h"
#include "DamageHooks.h"

bool __fastcall CEventDamage__AffectsPed_Hook(CEventDamage* This, SKIP_EDX, CPed* pPed)
{
    if (CLocalPlayer::GetIsHostingEntity(pPed))
    {
        return plugin::CallMethodAndReturn<bool, 0x4B35A0>(This, pPed);
    }
    return false;
}

void DamageHooks::InjectHooks()
{
   /* patch::RedirectCall(0x439055, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x4813CD, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x4BA06F, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x5680B8, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x568271, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x5E2F57, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x5E3020, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x5F0CBB, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x5F0F25, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x6049B1, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x604AA0, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x6297FB, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x65401F, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x656974, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x67C8FC, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x686033, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x68B44E, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x6A929D, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x6A94DF, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x6B6378, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x6B6566, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x6D83F5, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x6DB30B, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x6DBE7E, CEventDamage__AffectsPed_Hook);
    patch::RedirectCall(0x73A687, CEventDamage__AffectsPed_Hook);
    patch::SetPointer(0x85AD8C, CEventDamage__AffectsPed_Hook);*/
}