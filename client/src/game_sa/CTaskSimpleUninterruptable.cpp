#include "stdafx.h"
#include "CTaskSimpleUninterruptable.h"

// 0x43E2E0
CTaskSimpleUninterruptable::CTaskSimpleUninterruptable() : CTaskSimple(plugin::dummy)
{
    plugin::CallMethod<0x43E2E0, CTaskSimpleUninterruptable*>(this);
}
