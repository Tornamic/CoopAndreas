#include "stdafx.h"
#include "SaveLoadHooks.h"
#include "CGenericGameStorage.h"


char* __fastcall CSimpleVariablesSaveStructure__Construct_CText__Get_Hook(CText* This, int, char* key)
{
    const char* text = TheText.Get(key);
    std::string modifiedText = "(Coop) " + std::string(text);
    return _strdup(modifiedText.c_str());
}

// in the original implementation, R* write a ped if their type 
// is PED_TYPE_PLAYER1, network players are also PED_TYPE_PLAYER1, 
// so their data gets saved, which causes critical bugs when loading 
// the save again
bool CPools__SavePedPool_Reimpl()
{
    int playerCount = 1;
    CGenericGameStorage::_SaveDataToWorkBuffer(&playerCount, 4);

    CPlayerPed* ped = FindPlayerPed(0);
    int pedRef = CPools::GetPedRef(ped);
    int modelIndex = ped->m_nModelIndex;
    int pedType = ped->m_nPedType;

    CGenericGameStorage::_SaveDataToWorkBuffer(&pedRef,  4);
    CGenericGameStorage::_SaveDataToWorkBuffer(&modelIndex, 4);
    CGenericGameStorage::_SaveDataToWorkBuffer(&pedType, 4);
    plugin::CallMethod<0x5D57E0>(ped); // CPlayerPed::Save

    return true;
}

void SaveLoadHooks::InjectHooks()
{
    patch::RedirectCall(0x5D1BB2, CSimpleVariablesSaveStructure__Construct_CText__Get_Hook); // add [Coop] prefix to save display name

    patch::RedirectJump(0x5D4B40, CPools__SavePedPool_Reimpl);
}
