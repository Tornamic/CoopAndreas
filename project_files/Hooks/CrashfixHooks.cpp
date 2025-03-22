#include "stdafx.h"
#include "CrashfixHooks.h"

// i hope it will work
void __declspec(naked) CVehicleAnimGroup__ComputeAnimDoorOffsets_Hook()
{
    __asm
    {
        // ensure door id < 16
        mov edx, [esp + 8] // get door id
        cmp edx, 16 // compare
        jb cont // if lower than 16, good, exit the hook 

        // zero if out of range
        mov edx, 0
        mov[esp + 8], edx

        cont:
        // code cave
        mov     edx, [esp + 8]
        lea     eax, [edx + edx * 2]

        push 0x6E3D17
        ret
    }
}

void __declspec(naked) CAnimManager__BlendAnimation_Hook()
{
    __asm
    {
        mov eax, [esp + 4] // get RpClump*
        test eax, eax // check for nullptr

        jnz cont // jump if ok

        retn // exit if not ok

        cont:
        // code cave
        sub esp, 0x14
        mov ecx, [esp + 0x14 + 0x4]

        push 0x4D4617
        ret
    }
}

void CrashfixHooks::InjectHooks()
{
    patch::RedirectJump(0x6E3D10, CVehicleAnimGroup__ComputeAnimDoorOffsets_Hook);
    patch::RedirectJump(0x4D4610, CAnimManager__BlendAnimation_Hook);
}
