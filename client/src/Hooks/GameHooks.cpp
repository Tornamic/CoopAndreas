#include "stdafx.h"
#include <CCheatSync.h>
#include "GameHooks.h"
#include "CKeySync.h"
#include <CCutsceneMgr.h>
#include <CWeatherSync.h>
#include <CCoronas.h>

static void __cdecl CMenuManager__DrawFrontEnd_FixChat_Hook(float alpha)
{
    CFont::SetAlphaFade(alpha);

    if (CChat::m_bInputActive)
        CChat::ToggleInput(false);
}

static void __cdecl CClock__RestoreClock_Hook()
{
    CClock::RestoreClock();
    CWeatherSync::SyncCurrentState();
}

static void __cdecl CClock__SetGameClock_Hook(unsigned char h, unsigned char m, unsigned char d)
{
    CClock::SetGameClock(h, m, d);
    CWeatherSync::SyncCurrentState();
}

// CCheat::AddToCheatString picks the cheat function (0x43856A) with esi = the cheat id, which is popped before the
// two ends hooked below; keep it for them
static int nAppliedCheatId = -1;
static void __declspec(naked) ProcessCheat_KeepCheatId_Hook()
{
    __asm
    {
        mov nAppliedCheatId, esi
        mov eax, dword ptr [esi * 4 + 0x8A5B58]
        push 0x438571
        retn
    }
}

static void __declspec(naked) ProcessCheat_Hook1()
{
    __asm
    {
        mov byte ptr ds : [0x969110] , bl

        push nAppliedCheatId
        call CCheatSync::OnLocalCheat
        add esp, 4

        call CWeatherSync::SyncCurrentState

        push 0x438589
        retn
    }
}
static void __declspec(naked) ProcessCheat_Hook2()
{
    __asm
    {
        mov byte ptr ds : [0x969110] , bl

        push nAppliedCheatId
        call CCheatSync::OnLocalCheat
        add esp, 4

        call CWeatherSync::SyncCurrentState

        push 0x4385A3
        retn
    }
}

CEntity* pEntity = nullptr;
DWORD dwJmpAddress = 0x0;
DWORD CRenderer__AddEntityToRenderList_Hook_Continue = 0x5534B5;
DWORD CRenderer__AddEntityToRenderList_Hook_Return = 0x553533;
static void __declspec(naked) CRenderer__AddEntityToRenderList_Hook()
{
    __asm
    {
        mov pEntity, ecx
        pushad
    }
    if ((RwObject*)((DWORD)pEntity + 0x18) == nullptr)
    {
        __asm
        {
            popad
            retn
        }
    }
    else
    {
        __asm
        {
            popad
            push esi
            mov esi, [esp + 4 + 4]
            jmp CRenderer__AddEntityToRenderList_Hook_Continue
        }
    }
}

uintptr_t CTheZones__Update_Dest = 0x0;
CControllerState oldControllerState;

static void __cdecl CTheZones__Update_Hook()
{
    plugin::CallDyn(CTheZones__Update_Dest);

    if (!CNetwork::m_bAuthenticated)
    {
        return;
    }

    CPad* pad = CPad::GetPad(0);
    if (pad->DisablePlayerControls != 0)
    {
        // if disabled, dont send the event-based keysync packet, let the OnFootUpdate packet take care of it
        return;
    }

    CControllerState newState = pad->NewState;

    if (CUtil::CompareControllerStates(oldControllerState, newState))
        return;

    oldControllerState = newState;

    Packets::Players::KeyPressed keyPressed{};
    CKeySync::CollectState(keyPressed.keySnapshot);
    keyPressed.currentRotation = FindPlayerPed(0)->m_fCurrentRotation;
    keyPressed.aimingRotation = FindPlayerPed(0)->m_fAimingRotation;
    GetPacketFactory().Send(keyPressed);
}

int __purecall_Hook()
{
    *(char**)0xDEAD =
        "This hook is needed for a more detailed crash log when calling an unimplemented virtual function";
    *(char**)0xDEAD2 = "we will get a full backtrace instead of a single msgbox";
    __asm
    {
        mov eax, 0
        push 0x0
        ret
    }

    return 0;
}

static bool __fastcall CWeapon__FireSniper_Hook(CWeapon* This, SKIP_EDX, CPed* creator, CEntity* victim, CVector* target)
{
    if (creator != FindPlayerPed(0))
    {
        uint8_t moonSize = CCoronas::MoonSize;
        bool result = This->FireSniper(creator, victim, target);

        CCoronas::MoonSize = moonSize;
        return result;
    }

    uint8_t oldMoonSize = CCoronas::MoonSize;
    bool result = This->FireSniper(creator, victim, target);

    if (oldMoonSize != CCoronas::MoonSize)
    {
        Packets::World::UpdateMoonSize packet{};
        packet.moonSize = CCoronas::MoonSize;
        GetPacketFactory().Send(packet);
    }

    return result;
}

#ifdef DEBUG
bool __fastcall CPCKeyboard__GetKeyDown_Hook(int, int, uint16_t key_code, uint8_t use_mode, char* usage)
{
    return GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(key_code);
}

bool __fastcall CPCKeyboard__GetJustKeyDown_Hook(int, int, uint16_t key_code, uint8_t use_mode, char* usage)
{
    return GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(key_code) & 0x1;
}
#endif

void GameHooks::InjectHooks()
{
    patch::RedirectCall(0x57C2A3, CMenuManager__DrawFrontEnd_FixChat_Hook);

    patch::RedirectCall(0x47F1C7, CClock__RestoreClock_Hook);
    patch::RedirectCall(0x441534, CClock__SetGameClock_Hook);
    patch::RedirectJump(0x43856A, ProcessCheat_KeepCheatId_Hook);  // mov eax, [esi*4 + m_aCheatFunctions] (7 bytes)
    patch::RedirectJump(0x438583, ProcessCheat_Hook1);
    patch::RedirectJump(0x43859D, ProcessCheat_Hook2);

    // CRenderer::RenderEverythingBarRoads => CVisibilityPlugins::GetClumpAlpha crash fix
    patch::RedirectJump(0x5534B0, CRenderer__AddEntityToRenderList_Hook);

    // no, here we will not do anything with zones, here we will get and send player keys
    // it is necessary for the menu to be processed correctly
    CTheZones__Update_Dest = injector::GetBranchDestination(0x53BF49).as_int();
    patch::RedirectCall(0x53BF49, CTheZones__Update_Hook);

    patch::RedirectJump(PURECALL, __purecall_Hook);

    patch::RedirectCall(0x7424CB, CWeapon__FireSniper_Hook);

#ifdef DEBUG
    /*patch::ReplaceFunction(0x571980, CPCKeyboard__GetKeyDown_Hook);
    patch::ReplaceFunction(0x571970, CPCKeyboard__GetJustKeyDown_Hook);*/
#endif
}
