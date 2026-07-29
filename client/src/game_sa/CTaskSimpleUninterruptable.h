#pragma once

#include <CTaskSimple.h>

class PLUGIN_API CTaskSimpleUninterruptable : public CTaskSimple
{
public:
    CTaskSimpleUninterruptable();
};

VALIDATE_SIZE(CTaskSimpleUninterruptable, 0x8);
