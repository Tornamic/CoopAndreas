#include "stdafx.h"
#include <CGameLogic.h>

bool bDrivebyHelpMessageShown = false;

bool bSwitchingWeapon = false;
bool bSwitchingLeft = false;
bool bSwitchingRight = false;
CTaskSimpleGangDriveBy* pSwitchingDriveByTask = nullptr;

bool bSavedDisableFlag = false;

bool bInModControlledDriveby = false;

void ProcessWeaponSwitching(CAnimBlendAssociation* pAnim, void* pData)
{
    if (!bSwitchingWeapon)
        return;
    if (pData != pSwitchingDriveByTask)
        return;

    CPlayerPed* pPlayerPed = FindPlayerPed(0);

    bool bLeap = false;
    bool bNoWeapon = false;
    if (bSwitchingRight)
    {
        pPlayerPed->m_pPlayerData->m_nChosenWeapon = pPlayerPed->m_nActiveWeaponSlot + 1;
        while (true)
        {
            if (pPlayerPed->m_pPlayerData->m_nChosenWeapon > 12)
            {
                pPlayerPed->m_pPlayerData->m_nChosenWeapon = 0;
                if (bLeap)
                {
                    bNoWeapon = true;
                    break;
                }
                bLeap = true;
            }

            CWeapon& weapon = pPlayerPed->m_aWeapons[pPlayerPed->m_pPlayerData->m_nChosenWeapon];
            if (weapon.m_eWeaponType != WEAPON_UNARMED && weapon.HasWeaponAmmoToBeUsed() &&
                CWeaponInfo::GetWeaponInfo(weapon.m_eWeaponType, WEAPSKILL_STD)->m_nWeaponFire ==
                    WEAPON_FIRE_INSTANT_HIT)
            {
                break;
            }

            pPlayerPed->m_pPlayerData->m_nChosenWeapon++;
        }
    }
    else if (bSwitchingLeft)
    {
        pPlayerPed->m_pPlayerData->m_nChosenWeapon = pPlayerPed->m_nActiveWeaponSlot - 1;
        while (true)
        {
            if (pPlayerPed->m_pPlayerData->m_nChosenWeapon < 0)
            {
                pPlayerPed->m_pPlayerData->m_nChosenWeapon = 12;
                if (bLeap)
                {
                    bNoWeapon = true;
                    break;
                }
                bLeap = true;
            }

            CWeapon& weapon = pPlayerPed->m_aWeapons[pPlayerPed->m_pPlayerData->m_nChosenWeapon];
            if (weapon.m_eWeaponType != WEAPON_UNARMED && weapon.HasWeaponAmmoToBeUsed() &&
                CWeaponInfo::GetWeaponInfo(weapon.m_eWeaponType, WEAPSKILL_STD)->m_nWeaponFire ==
                    WEAPON_FIRE_INSTANT_HIT)
            {
                break;
            }

            pPlayerPed->m_pPlayerData->m_nChosenWeapon--;
        }
    }
    if (bNoWeapon)
    {
        pPlayerPed->m_pPlayerData->m_nChosenWeapon = 0;
    }
    if (pPlayerPed->m_pPlayerData->m_nChosenWeapon != pPlayerPed->m_nActiveWeaponSlot)
    {
        pPlayerPed->RemoveWeaponAnims(pPlayerPed->m_nActiveWeaponSlot, -1000.0f);
        pPlayerPed->MakeChangesForNewWeapon(pPlayerPed->m_pPlayerData->m_nChosenWeapon);
    }
    bSwitchingWeapon = false;
    bSwitchingRight = false;
    bSwitchingLeft = false;
    pSwitchingDriveByTask = nullptr;
    CPad* pPad = CPad::GetPad(0);
    pPad->bDisablePlayerEnterCar = bSavedDisableFlag;
    TheCamera.m_bWaitForInterpolToFinish = false;
    if (!bNoWeapon)
    {
        CDriveBy::StartDriveby(pPlayerPed);
    }
}

void CTaskSimpleGangDriveBy__FinishAnimCB(CAnimBlendAssociation* pAnim, void* pData)
{
    ProcessWeaponSwitching(pAnim, pData);
}

void CDriveBy::Process(CPlayerPed* pPlayerPed)
{
    bool bIsInDriveby = false;
    CTask* pTask = pPlayerPed->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[TASK_PRIMARY_PRIMARY];
    if (pTask && pTask->GetTaskType() == eTaskType::TASK_SIMPLE_GANG_DRIVEBY)
    {
        bIsInDriveby = true;
    }

    if (bIsInDriveby && !bInModControlledDriveby)
    {
        return;
    }

    if (bIsInDriveby)
    {
        if (pPlayerPed->GetPadFromPlayer()->HornJustDown())
        {
            CDriveBy::StopDriveby(pPlayerPed);
            return;
        }
    }
    if (!bIsInDriveby)
    {
        bool isPassenger = pPlayerPed->m_nPedFlags.bInVehicle && pPlayerPed->m_pVehicle &&
                           pPlayerPed->m_pVehicle->m_pDriver != pPlayerPed;
        if (isPassenger)
        {
            if (!bDrivebyHelpMessageShown)
            {
                CHud::SetHelpMessage("Press ~k~~VEHICLE_HORN~ to toggle driveby mode on or off.", false, false, false);
                bDrivebyHelpMessageShown = true;
            }
        }
        if (isPassenger && pPlayerPed->GetPadFromPlayer()->HornJustDown())
        {
            pTask = CDriveBy::StartDriveby(pPlayerPed);
            if (pTask)
            {
                bIsInDriveby = true;
            }
        }
    }

    if (bIsInDriveby)
    {
        if (!bSwitchingWeapon)
        {
            CPad* pPad = CPad::GetPad(0);
            CWeapon& weapon = pPlayerPed->GetWeapon();
            bSwitchingLeft = pPad->CycleWeaponLeftJustDown();
            bSwitchingRight = pPad->CycleWeaponRightJustDown();

            if (weapon.m_nState == WEAPONSTATE_OUT_OF_AMMO)
            {
                bSwitchingRight = true;
            }

            if (weapon.m_nState != WEAPONSTATE_FIRING && (bSwitchingLeft || bSwitchingRight))
            {
                pSwitchingDriveByTask = (CTaskSimpleGangDriveBy*)pTask;
                bSwitchingWeapon = true;
                bSavedDisableFlag = pPad->bDisablePlayerEnterCar;
                pPad->bDisablePlayerEnterCar = true;
                CDriveBy::StopDriveby(pPlayerPed);
                TheCamera.m_bWaitForInterpolToFinish = true;
            }
        }
    }
}

bool CDriveBy::IsPedInDriveby(CPed* ped)
{
    CTask* pTask = ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[TASK_PRIMARY_PRIMARY];
    if (pTask && pTask->GetTaskType() == eTaskType::TASK_SIMPLE_GANG_DRIVEBY)
    {
        return true;
    }
    return false;
}

CTask* CDriveBy::StartDriveby(CPed* ped)
{
    if (ped == FindPlayerPed(0))
    {
        if (CWeaponInfo::GetWeaponInfo(ped->GetWeapon().m_eWeaponType, WEAPSKILL_STD)->m_nWeaponFire !=
            WEAPON_FIRE_INSTANT_HIT)
        {
            bool bLeap = false;
            ped->m_pPlayerData->m_nChosenWeapon = ped->m_nActiveWeaponSlot + 1;
            while (true)
            {
                if (ped->m_pPlayerData->m_nChosenWeapon > 12)
                {
                    ped->m_pPlayerData->m_nChosenWeapon = 0;
                    if (bLeap)
                    {
                        return nullptr;
                    }
                    bLeap = true;
                }

                CWeapon& weapon = ped->m_aWeapons[ped->m_pPlayerData->m_nChosenWeapon];
                if (weapon.HasWeaponAmmoToBeUsed() &&
                    CWeaponInfo::GetWeaponInfo(weapon.m_eWeaponType, WEAPSKILL_STD)->m_nWeaponFire ==
                        WEAPON_FIRE_INSTANT_HIT)
                {
                    break;
                }

                ped->m_pPlayerData->m_nChosenWeapon++;
            }
            if (ped->m_pPlayerData->m_nChosenWeapon != ped->m_nActiveWeaponSlot)
            {
                ped->RemoveWeaponAnims(ped->m_nActiveWeaponSlot, -1000.0f);
                ((CPlayerPed*)ped)->MakeChangesForNewWeapon(ped->m_pPlayerData->m_nChosenWeapon);
            }
        }
    }
    CTaskSimpleGangDriveBy* pTask = new CTaskSimpleGangDriveBy(nullptr, nullptr, 0.0f, 100, DRIVEBY_AI_ALL_DIRN, true);
    if (pTask)
    {
        ped->m_pIntelligence->m_TaskMgr.SetTask(pTask, TASK_PRIMARY_PRIMARY, false);
        bInModControlledDriveby = true;
    }
    ped->SetCurrentWeapon(ped->m_nActiveWeaponSlot);
    return pTask;
}

void CDriveBy::StopDriveby(CPed* ped)
{
    CTask* pTask = ped->m_pIntelligence->m_TaskMgr.m_aPrimaryTasks[TASK_PRIMARY_PRIMARY];
    if (pTask && pTask->GetTaskType() == eTaskType::TASK_SIMPLE_GANG_DRIVEBY)
    {
        pTask->MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
    }
    bInModControlledDriveby = false;
}

void CTaskSimpleGangDriveBy__FinishAnimCB2(CAnimBlendAssociation* pAnim, void* pData)
{
    CTaskSimpleGangDriveBy__FinishAnimCB(pAnim, pData);
    plugin::Call<0x621BE0>(pAnim, pData);
}

static void __declspec(naked) SetDeleteCallback_hook1()
{
    __asm
    {
        push esi
        push offset CTaskSimpleGangDriveBy__FinishAnimCB
        push 0x62D389
        retn
    }
}

static void __declspec(naked) SetDeleteCallback_hook2()
{
    __asm
    {
        mov eax, [esi + 0x2C]
        push eax

        push offset CTaskSimpleGangDriveBy__FinishAnimCB
        push 0x627BE8
        retn
    }
}

static bool __fastcall CPad__GetLookLeft_Reimpl(CPad* pPad, SKIP_EDX)
{
    if (CWorld::PlayerInFocus == 0 && bSwitchingWeapon)
    {
        return false;
    }

    if (pPad->DisablePlayerControls)
    {
        return false;
    }

    if (pPad->NewState.LeftShoulder2 && !pPad->OldState.LeftShoulder2)
    {
        return false;
    }

    if (!pPad->NewState.RightShoulder2 && pPad->OldState.RightShoulder2)
    {
        return false;
    }

    if (pPad->NewState.LeftShoulder2 && !pPad->NewState.RightShoulder2)
    {
        return true;
    }
    return false;
}

static bool __fastcall CPad__GetLookRight_Reimpl(CPad* pPad, SKIP_EDX)
{
    if (CWorld::PlayerInFocus == 0 && bSwitchingWeapon)
    {
        return false;
    }

    if (pPad->DisablePlayerControls)
    {
        return false;
    }

    if (pPad->NewState.RightShoulder2 && !pPad->OldState.RightShoulder2)
    {
        return false;
    }

    if (!pPad->NewState.LeftShoulder2 && pPad->OldState.LeftShoulder2)
    {
        return false;
    }

    if (pPad->NewState.RightShoulder2 && !pPad->NewState.LeftShoulder2)
    {
        return true;
    }
    return false;
}

void CDriveBy::InitHooks()
{
    patch::SetPointer(0x627C14 + 1, CTaskSimpleGangDriveBy__FinishAnimCB2);
    patch::RedirectJump(0x62D382, SetDeleteCallback_hook1);
    patch::RedirectJump(0x627BE1, SetDeleteCallback_hook2);
    patch::ReplaceFunction(0x53FDD0, CPad__GetLookLeft_Reimpl);
    patch::ReplaceFunction(0x53FE10, CPad__GetLookRight_Reimpl);
}
