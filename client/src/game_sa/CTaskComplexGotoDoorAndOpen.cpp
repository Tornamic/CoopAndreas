#include "stdafx.h"
#include "CTaskComplexGotoDoorAndOpen.h"

// 0x66BB20
CTaskComplexGotoDoorAndOpen::CTaskComplexGotoDoorAndOpen(CObject* door) : CTaskComplex(plugin::dummy)
{
    plugin::CallMethod<0x66BB20, CTaskComplexGotoDoorAndOpen*, CObject*>(this, door);
}

// 0x66BBA0
CTaskComplexGotoDoorAndOpen::CTaskComplexGotoDoorAndOpen(const CVector& start, const CVector& end)
    : CTaskComplex(plugin::dummy)
{
    plugin::CallMethod<0x66BBA0, CTaskComplexGotoDoorAndOpen*, const CVector&, const CVector&>(this, start, end);
}
