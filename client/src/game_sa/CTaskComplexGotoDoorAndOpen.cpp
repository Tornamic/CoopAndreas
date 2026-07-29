#include "stdafx.h"
#include "CTaskComplexGotoDoorAndOpen.h"

CTaskComplexGotoDoorAndOpen::CTaskComplexGotoDoorAndOpen(CObject* door) : CTaskComplex(plugin::dummy)
{
    plugin::CallMethod<0x66BB20, CTaskComplexGotoDoorAndOpen*, CObject*>(this, door);
}

CTaskComplexGotoDoorAndOpen::CTaskComplexGotoDoorAndOpen(const CVector& start, const CVector& end)
    : CTaskComplex(plugin::dummy)
{
    plugin::CallMethod<0x66BBA0, CTaskComplexGotoDoorAndOpen*, const CVector&, const CVector&>(this, start, end);
}
